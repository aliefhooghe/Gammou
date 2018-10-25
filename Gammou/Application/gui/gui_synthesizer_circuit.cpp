
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

        bool abstract_gui_circuit::save_state(Persistence::circuit_state& state)
        {
			std::vector<abstract_gui_component*> tmp;
			tmp.reserve(m_widgets.size());

			for (auto& w : m_widgets)
				tmp.push_back(w.get());

			save_components(tmp, state);
            return true;
        }

        bool abstract_gui_circuit::load_state(const Persistence::circuit_state& state)
        {
            reset_content();
			insert_components(state);
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

            lock_circuit();
            for (auto component : to_remove)
                remove_widget(component);
            unlock_circuit();
        }

		bool abstract_gui_circuit::on_key_down(const View::keycode key)
		{
			if (get_key_state(View::key_ctrl)) {

				if (key == View::key_C) {			//	COPY
					save_components(get_selection(), m_clipboard);
				}
				else if (key == View::key_V) {		//	PASTE
					//	translate
					for (auto& component : m_clipboard.components) {
						component.x_pos += 100;	//	TODO : make component appear near cursor
						component.y_pos += 100;
					}
					insert_components(m_clipboard);
				}

			}

			return true;
		}

		void abstract_gui_circuit::insert_components(const Persistence::circuit_state & state)
		{
			const unsigned int component_count =
				static_cast<unsigned int>(state.components.size());

			// record_id -> component association
			std::vector<abstract_gui_component*> loaded_gui_components;
			loaded_gui_components.reserve(component_count);

			// Loading components (This add them on circuit)
			for (const auto& component_state : state.components)
				loaded_gui_components.push_back(load_component(component_state));

			// Loading links
			for (const auto& link : state.links) {
				// Check Record Id
				if (link.src_record_id >= component_count ||
					link.dst_record_id >= component_count)
					throw std::domain_error("Invalid Record Id\n");

				// Get
				abstract_gui_component *src = loaded_gui_components[link.src_record_id];
				abstract_gui_component *dst = loaded_gui_components[link.dst_record_id];

				// connect function manage the lock and input-output id checking
				connect(src, link.output_id, dst, link.input_id);
			}
		}

        void abstract_gui_circuit::save_component(
            Persistence::component_state& state,
            abstract_gui_component *component)
        {
            state.factory_id = component->get_sound_component_factory_id();
            state.x_pos = component->get_x();
            state.y_pos = component->get_y();

            state.data.resize(0);
            Persistence::buffer_output_stream buffer(state.data);

            component->save_sound_component_state(buffer);
        }

        abstract_gui_component *abstract_gui_circuit::load_component(const Persistence::component_state& state)
        {

            if (state.factory_id == Persistence::INTERNAL_FACTORY_ID) {
                // Component is an internal component

                // Check that there is a 4-Byte data record (The internal Id)
                if(state.data.size() != sizeof(uint32_t))
                    throw std::domain_error("Invalid Internal Components data");

                // get the internal Id
                uint32_t internal_id;
                std::memcpy(&internal_id, state.data.data(), sizeof(uint32_t));

                // Get the component
                abstract_gui_component *component =
                    gui_component_by_internal_id(internal_id);

                // Set the Component position
                component->set_pos(
                    state.x_pos,
                    state.y_pos
                );

                // Return The gui component ptr
                return component;
            }
            else if(m_complete_component_factory.check_factory_presence(state.factory_id)){
                Persistence::buffer_input_stream cdata(state.data);

                // Build component from data
                std::unique_ptr<gui_sound_component>
                    component = m_complete_component_factory.get_new_gui_component(
                        state.factory_id,
                        state.x_pos,
                        state.y_pos,
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
