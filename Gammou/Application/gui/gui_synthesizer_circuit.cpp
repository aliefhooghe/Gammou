
#include <map>

#include "gui_synthesizer_circuit.h"
#include "plugin_request_dialog.h"

namespace Gammou {

	namespace Gui {

        /*
                abstract gui circuit implementation
        */

        abstract_gui_circuit::abstract_gui_circuit(
            gui_component_main_factory& factory,
            const unsigned int components_channel_count,
            const int x,
            const int y,
            const unsigned int width,
            const unsigned int height,
            const View::color background)
        :   abstract_gui_component_map(x, y, width, height, background),
            m_complete_component_factory(factory),
            m_components_channel_count(components_channel_count),
            m_creation_factory_id(Sound::NO_FACTORY)
        {

        }

        abstract_gui_circuit::abstract_gui_circuit(
            gui_component_main_factory& factory,
            const unsigned int components_channel_count,
            const View::rectangle& rect,
            const View::color background)
        :   abstract_gui_component_map(rect, background),
            m_complete_component_factory(factory),
            m_components_channel_count(components_channel_count),
            m_creation_factory_id(Sound::NO_FACTORY)
        {

        }

        abstract_gui_circuit::~abstract_gui_circuit()
        {
        }

        bool abstract_gui_circuit::on_mouse_dbl_click(const int x, const int y)
        {

            if (!abstract_gui_component_map::on_mouse_dbl_click(x, y)){

                abstract_gui_component *focused_component = get_focused_widget();

                if (focused_component != nullptr
                    && focused_component->get_sound_component_factory_id() != Persistence::INTERNAL_FACTORY_ID) {
                    DEBUG_PRINT("Delete component '%s'\n", focused_component->get_component()->get_name().c_str());
                    lock_circuit();
                    remove_widget(focused_component);
                    unlock_circuit();
                    DEBUG_PRINT("OK\n");
                }
                else if (m_creation_factory_id != Sound::NO_FACTORY) {

                    const auto& request_form =
                        m_complete_component_factory.get_plugin_request_form(m_creation_factory_id);

                    std::unique_ptr<Sound::answer_form> answer_form;

                    //	If request_form is empty
                    if (std::holds_alternative<Sound::empty_request_form>(request_form)) {
                        answer_form = std::make_unique<Sound::answer_form>
                            (Sound::empty_answer_form{});
                    }
                    else{
                        auto& request_list =
                            std::get<Sound::request_list>(request_form);

                        plugin_request_dialog dialog(request_list);
                        dialog.show("Plugin Properties");

                        DEBUG_PRINT("Getting answer form\n");
                        answer_form = dialog.get_answer_form();
                    }

                    DEBUG_PRINT("Creating a %u-channel component\n", m_components_channel_count);

                    try {
                        std::unique_ptr<gui_sound_component>
                            component =
                                m_complete_component_factory.get_new_gui_component(
                                    m_creation_factory_id,
                                    convert_x(static_cast<unsigned int>(x)),
                                    convert_y(static_cast<unsigned int>(y)),
                                    *answer_form,
                                    m_components_channel_count);

                        add_sound_component_to_frame(&(component->get_sound_component()));
                        add_gui_component(std::move(component));
                    }
                    catch(...) {
                        DEBUG_PRINT("Failed to create Component\n");
                    }

                }
            }

            return true;
        }

        void abstract_gui_circuit::select_component_creation_factory_id(const unsigned int factory_id)
        {
            DEBUG_PRINT("Set Factory Id = %u\n", factory_id);

            if (m_complete_component_factory.check_factory_presence(factory_id))
                m_creation_factory_id = factory_id;
            else
                DEBUG_PRINT("Unregisterd Factory");
        }

        bool abstract_gui_circuit::save_state(Sound::data_output_stream & data)
        {
            Persistence::circuit_state_header record_header;

            // component -> record_id association
            std::map<abstract_gui_component*, uint32_t> component_record_id;

            uint32_t component_counter = 0u;
            uint32_t link_counter = 0u;

            DEBUG_PRINT("CIRCUIT SAVE STATE\n");

            //	We skip header (it will be written after)
            const unsigned int start_pos = data.tell();
            data.seek(sizeof(record_header));

            // save components

            for (auto& component : m_widgets) {
                // Save each component
                abstract_gui_component *component_ptr =
                    &(*component);

                component_record_id[component_ptr] = component_counter;
                save_component(data, component_ptr);
                component_counter++;
            }

            // save links

            for (auto& component : m_widgets) {
                // component is dst

                const unsigned int ic = component->get_component()->get_input_count();

                //	Getting component record_id
                const uint32_t dst_record_id = component_record_id[&(*component)];

                for (unsigned int input_id = 0; input_id < ic; ++input_id) {
                    // for each dst input
                    unsigned int output_id;
                    abstract_gui_component *src = get_input_src(component, input_id, output_id);

                    // something linked to input
                    if (src != nullptr) {
                        const uint32_t src_record_id = component_record_id[src];
                        save_link(data, src_record_id, output_id, dst_record_id, input_id);
                        link_counter++;
                    }
                }
            }

            record_header.component_count = component_counter;
            record_header.link_count = link_counter;

            // save header
            const unsigned int current_pos = data.tell();

            data.seek(start_pos, Gammou::Sound::abstract_data_stream::seek_mode::SET);
            data.write(&record_header, sizeof(record_header));
            data.seek(current_pos, Gammou::Sound::abstract_data_stream::seek_mode::SET);

            return true;
        }

        bool abstract_gui_circuit::load_state(Sound::data_input_stream & data)
        {
            //	Delete current content
            reset_content();

            //---
            Persistence::circuit_state_header record_header;

            // Reading header
            data.read(&record_header, sizeof(record_header));

            DEBUG_PRINT("CIRCUIT LOAD STATE : \n");
            DEBUG_PRINT("  -> %u components\n", record_header.component_count);
            DEBUG_PRINT("  -> %u links\n", record_header.link_count);

            // record_id -> component association
            std::vector<abstract_gui_component*> loaded_gui_components(record_header.component_count);

            // Loading components (This add them on circuit)
            for (unsigned int i = 0; i < record_header.component_count; ++i)
                loaded_gui_components[i] = load_component(data);

            // Loading links
            for (unsigned int i = 0; i < record_header.link_count; ++i) {
                Persistence::link_state link;

                DEBUG_PRINT("LOAD LINK\n");

                // Read link
                data.read(&link, sizeof(link));

                // Check Record Id
                if (link.src_record_id >= loaded_gui_components.size() ||
                    link.dst_record_id >= loaded_gui_components.size())
                    throw std::domain_error("Invalid Record Id\n");

                // Get
                abstract_gui_component *src = loaded_gui_components[link.src_record_id];
                abstract_gui_component *dst = loaded_gui_components[link.dst_record_id];

                // connect function manage the lock and input-output id checking
                connect(src, link.output_id, dst, link.input_id);
            }

            return true;
        }

        void abstract_gui_circuit::reset_content()
        {
            std::deque<abstract_gui_component*> to_remove;

            for (auto& component : m_widgets) {
                if (component->get_sound_component_factory_id()
                    != Persistence::INTERNAL_FACTORY_ID) {
                    to_remove.push_back(component.get());
                }
            }

            for (auto component : to_remove)
                remove_widget(component);
        }

        void abstract_gui_circuit::save_component(Sound::data_output_stream& data, abstract_gui_component * component)
        {
            Persistence::component_state_header record_header;
            Persistence::buffer_stream sound_component_state;

            record_header.factory_id = component->get_sound_component_factory_id();
            record_header.gui_x_pos = component->get_x();
            record_header.gui_y_pos = component->get_y();
            record_header.data_size = component->save_sound_component_state(sound_component_state);

            DEBUG_PRINT("COMPONENT SAVE STATE (factory id = %u)\n", record_header.factory_id);

            data.write(&record_header, sizeof(record_header));	//	Write header
            sound_component_state.flush_data(data);				//	Write Sound Component data
        }

        abstract_gui_component *abstract_gui_circuit::load_component(Sound::data_input_stream & data)
        {
            Persistence::component_state_header record_header;

            // Read header
            data.read(&record_header, sizeof(record_header));

            if (record_header.factory_id == Persistence::INTERNAL_FACTORY_ID) {
                // Component is an internal component

                // Check that ther is a 4-Byte data record (The internal Id)
                if(record_header.data_size != sizeof(uint32_t))
                    throw std::domain_error("Invalid Internal Components data");

                // Load the internal Id
                uint32_t internal_id;
                if( data.read(&internal_id, sizeof(uint32_t)) != sizeof(uint32_t))
                    throw std::domain_error("Read Error");

                // Get the component
                abstract_gui_component *component = gui_component_by_internal_id(internal_id);

                // Set the Component position
                component->set_pos(
                    record_header.gui_x_pos,
                    record_header.gui_y_pos
                );

                // Return The guicomponent
                return component;
            }
            else if(m_complete_component_factory.check_factory_presence(record_header.factory_id)){
                Persistence::constrained_input_stream cdata(data, record_header.data_size);

                // Build component from data
                std::unique_ptr<gui_sound_component>
                    component = m_complete_component_factory.get_new_gui_component(
                        record_header.factory_id,
                        record_header.gui_x_pos,
                        record_header.gui_y_pos,
                        cdata,
                        m_components_channel_count);

                //	Add process component on frame
                lock_circuit();
                add_sound_component_to_frame(&(component->get_sound_component()));
                unlock_circuit();

                abstract_gui_component *ret = component.get();
                add_gui_component(std::move(component));
                return ret;
            }
            else {
                // Todo dummy component
                throw std::domain_error("Unknown factory !!!");
            }

        }

        void abstract_gui_circuit::save_link(Sound::data_output_stream& data, const unsigned int src_record_id,
            const unsigned int output_id, const unsigned int dst_record_id, const unsigned int input_id)
        {
            Persistence::link_state link;

            DEBUG_PRINT("LINK SAVE\n");

            link.src_record_id = static_cast<uint32_t>(src_record_id);
            link.output_id = static_cast<uint32_t>(output_id);
            link.dst_record_id = static_cast<uint32_t>(dst_record_id);
            link.input_id = static_cast<uint32_t>(input_id);

            data.write(&link, sizeof(link));
        }

		/*
				abstract gui synthesizer circuit implementation
		*/

		abstract_gui_synthesizer_circuit::abstract_gui_synthesizer_circuit(
            gui_component_main_factory& factory,
			const unsigned int components_channel_count,
			Sound::synthesizer * synthesizer, 
			std::mutex * synthesizer_mutex, 
            const int x,
            const int y,
            const unsigned int width,
            const unsigned int height,
            const View::color background)
        :   abstract_gui_circuit(
                factory,
                components_channel_count,
                x, y, width, height, background),
            m_synthesizer(synthesizer),
            m_synthesizer_mutex(synthesizer_mutex)
		{
		}

        abstract_gui_synthesizer_circuit::abstract_gui_synthesizer_circuit(
            gui_component_main_factory& factory,
            const unsigned int components_channel_count,
            Sound::synthesizer * synthesizer,
            std::mutex * synthesizer_mutex,
            const View::rectangle& rect,
            const View::color background)
        :   abstract_gui_circuit(
                factory,
                components_channel_count,
                rect, background),
            m_synthesizer(synthesizer),
            m_synthesizer_mutex(synthesizer_mutex)
        {
        }

        void abstract_gui_synthesizer_circuit::lock_circuit()
        {
            m_synthesizer_mutex->lock();
        }

        void abstract_gui_synthesizer_circuit::unlock_circuit()
        {
            m_synthesizer_mutex->unlock();
        }


        /*
                internal gui component implementation
        */

        internal_gui_component::internal_gui_component(
            Process::abstract_component<double> *component,
            const uint32_t internal_id,
            const int x,
            const int y)
        :   abstract_gui_component(
                x, y,
                component->get_input_count(),
                component->get_output_count()),
            m_internal_component_id(internal_id),
            m_component(component)
        {
        }

        internal_gui_component::~internal_gui_component()
        {
        }

        Process::abstract_component<double> *internal_gui_component::get_component() const
        {
            return m_component;
        }

        unsigned int internal_gui_component::get_sound_component_factory_id() const
        {
            return Persistence::INTERNAL_FACTORY_ID;
        }

        unsigned int internal_gui_component::save_sound_component_state(Sound::data_output_stream& data)
        {
            uint32_t id = m_internal_component_id; // for constness
            return data.write(&id, sizeof(uint32_t));
        }


	} /* Gui */

} /* Gammou */
