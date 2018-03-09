
#include <map>

#include "gui_synthesizer_circuit.h"

namespace Gammou {

	namespace Gui {

		/*
				abstract gui synthesizer circuit implementation
		*/

		abstract_gui_synthesizer_circuit::abstract_gui_synthesizer_circuit(
			gui_component_main_factory *complete_component_factory,
			const unsigned int components_channel_count,
			Sound::synthesizer * synthesizer, 
			std::mutex * synthesizer_mutex, 
			unsigned int x, const unsigned int y, const unsigned int width, const unsigned int height, const View::color background)
			: abstract_gui_component_map(synthesizer_mutex, x, y, width, height, background),
			m_synthesizer(synthesizer),
			m_synthesizer_mutex(synthesizer_mutex),
			m_complete_component_factory(complete_component_factory),
			m_components_channel_count(components_channel_count),
			m_creation_factory_id(Sound::NO_FACTORY)
		{
		}

		abstract_gui_synthesizer_circuit::abstract_gui_synthesizer_circuit(
			gui_component_main_factory *complete_component_factory,
			const unsigned int components_channel_count,
			Sound::synthesizer * synthesizer,
			std::mutex * synthesizer_mutex, const View::rectangle & rect, const View::color background)
			: abstract_gui_component_map(synthesizer_mutex, rect, background),
			m_synthesizer(synthesizer),
			m_synthesizer_mutex(synthesizer_mutex),
			m_complete_component_factory(complete_component_factory),
			m_components_channel_count(components_channel_count),
			m_creation_factory_id(Sound::NO_FACTORY)
		{
		}

		bool abstract_gui_synthesizer_circuit::on_mouse_dbl_click(const int x, const int y)
		{

			if (!abstract_gui_component_map::on_mouse_dbl_click(x, y) 
				&& m_creation_factory_id != Sound::NO_FACTORY) {

				const Sound::abstract_request_form& requests = m_complete_component_factory->get_plugin_request_form(m_creation_factory_id);
				const Sound::abstract_request_form::type type = requests.get_type();

				if (type != Sound::abstract_request_form::type::EMPTY) // TODO Handle requests
					throw std::runtime_error("Factory Request not handled!");
			
				const Sound::empty_answer_form answer;

				DEBUG_PRINT("Creating a %u-channel component\n", m_components_channel_count);

				gui_component_main_factory::complete_component
					component = m_complete_component_factory->get_new_complete_component(
						m_creation_factory_id,
						convert_x(x),
						convert_y(y),
						answer,
						m_components_channel_count
					);

				lock_circuit();
				add_sound_component_to_frame(component.second);
				unlock_circuit();

				add_gui_component(component.first);	
			}

			return true;
		}

		void abstract_gui_synthesizer_circuit::select_component_creation_factory_id(const unsigned int factory_id)
		{
			DEBUG_PRINT("Set Factory Id = %u\n", factory_id);

			if (m_complete_component_factory->check_factory_presence(factory_id))
				m_creation_factory_id = factory_id;
			else
				DEBUG_PRINT("Unregisterd Factory");
		}

		bool abstract_gui_synthesizer_circuit::save_state(Sound::data_sink & data)
		{
			Persistence::circuit_record_header record_header;

			// component -> record_id association
			std::map<abstract_gui_component*, uint32_t> component_record_id;

			uint32_t component_counter = 0u;
			uint32_t link_counter = 0u;

			DEBUG_PRINT("CIRCUIT SAVE STATE\n");

			//	We skip header 
			const unsigned int start_pos = data.tell();
			data.seek(sizeof(record_header));
			
			// save components

			for (abstract_gui_component *component : m_widgets) {
				// Check that component is not an internal circuit process component
				if (component->get_sound_component_factory_id() != Sound::NO_FACTORY) {
					component_record_id[component] = component_counter;
					save_component(data, component);
					component_counter++;
				}
			}

			// save links

			for (abstract_gui_component *component : m_widgets) {

				const unsigned int ic = component->get_component()->get_input_count();
				uint32_t dst_record_id;

				//	Getting dst component record_id
				if (component->get_sound_component_factory_id() == Sound::NO_FACTORY) {
					const uint8_t internal_id = get_component_internal_id(component);
					dst_record_id = Persistence::record_id_by_internal_component_id(internal_id);
				}
				else {
					dst_record_id = component_record_id[component];
				}

				for (unsigned int input_id = 0; input_id < ic; ++input_id) {
					unsigned int output_id;
					abstract_gui_component *src = get_input_src(component, input_id, output_id);
							
					// something linked to input
					if (src != nullptr) {
						unsigned int src_record_id;
					
						if (src->get_sound_component_factory_id() == Sound::NO_FACTORY) { //	src is an internal component 
							const uint8_t internal_id = get_component_internal_id(src);
							src_record_id = Persistence::record_id_by_internal_component_id(internal_id);
						}
						else { // src is factory-builded sound_component
							src_record_id = component_record_id[src];
						}

						save_link(data, src_record_id, output_id, dst_record_id, input_id);
						link_counter++;
					}
				}
			}
			
			record_header.component_count = component_counter;
			record_header.link_count = link_counter;

			// save header 
			const unsigned int current_pos = data.tell();

			data.seek(start_pos, Gammou::Sound::data_stream::seek_mode::SET);
			data.write(&record_header, sizeof(record_header));
			data.seek(current_pos, Gammou::Sound::data_stream::seek_mode::SET);

			return true;
		}

		bool abstract_gui_synthesizer_circuit::load_state(Sound::data_source & data)
		{
			//	Delete current content
			reset_content();
			
			//---
			Persistence::circuit_record_header record_header;
			
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
				Persistence::link_record link;

				DEBUG_PRINT("LOAD LINK\n");

				// Read link
				data.read(&link, sizeof(link));

				abstract_gui_component *src = nullptr;
				abstract_gui_component *dst = nullptr;


				//	if link src is an internal circuit component				
				if (Persistence::record_id_is_internal_component(link.src_record_id)) {
					const uint8_t internal_component_id = Persistence::internal_component_id_by_record_id(link.src_record_id);
					src = gui_component_by_internal_id(internal_component_id);
				}
				else {	//	link src is a factory builded component
					src = loaded_gui_components[link.src_record_id];
				}

				// Idem for dst
				if (Persistence::record_id_is_internal_component(link.dst_record_id)) {
					const uint8_t internal_component_id = Persistence::internal_component_id_by_record_id(link.dst_record_id);
					dst = gui_component_by_internal_id(internal_component_id);
				}
				else {
					dst = loaded_gui_components[link.dst_record_id];
				}

				// connect function manage the lock.
				
				connect(src, link.output_id, dst, link.input_id);
			}

			return true;
		}

		void abstract_gui_synthesizer_circuit::save_component(Sound::data_sink& data, abstract_gui_component * component)
		{
			if (component->get_sound_component_factory_id() == Sound::NO_FACTORY)
				throw std::domain_error("It is impossible to save a 'NO_FACTORY' component's state");

			Persistence::component_record_header record_header;
			Persistence::buffer_data_sink sound_component_state;

			record_header.factory_id = component->get_sound_component_factory_id();
			record_header.gui_x_pos = component->get_x();
			record_header.gui_y_pos = component->get_y();
			record_header.data_size = component->save_sound_component_state(sound_component_state);

			DEBUG_PRINT("COMPONENT SAVE STATE (factory id = %u)\n", record_header.factory_id);

			data.write(&record_header, sizeof(record_header));	//	Write header
			sound_component_state.flush_data(data);				//	Write Sound Component data
		}

		abstract_gui_component *abstract_gui_synthesizer_circuit::load_component(Sound::data_source & data)
		{
			Persistence::component_record_header record_header;

			// Read header
			data.read(&record_header, sizeof(record_header));

			// We do not have this plugin factory
			if (! m_complete_component_factory->check_factory_presence(record_header.factory_id)) {
				// Todo dummy component
				throw std::domain_error("Unknown factory !!!");
			}

			// To do Restriction on data size, via decorateur
			// Build component from data

			gui_component_main_factory::complete_component
				component = m_complete_component_factory->get_new_complete_component(
					record_header.factory_id,
					record_header.gui_x_pos,
					record_header.gui_y_pos,
					data,
					m_components_channel_count);

			//	Add process component on frame
			lock_circuit();
			add_sound_component_to_frame(component.second);
			unlock_circuit();

			add_gui_component(component.first);

			return component.first;
		}

		void abstract_gui_synthesizer_circuit::save_link(Sound::data_sink& data, const unsigned int src_record_id, 
			const unsigned int output_id, const unsigned int dst_record_id, const unsigned int input_id)
		{
			Persistence::link_record link;

			DEBUG_PRINT("LINK SAVE\n");

			link.src_record_id = static_cast<uint32_t>(src_record_id);
			link.output_id = static_cast<uint32_t>(output_id);
			link.dst_record_id = static_cast<uint32_t>(dst_record_id);
			link.input_id = static_cast<uint32_t>(input_id);

			data.write(&link, sizeof(link));
		}

		void abstract_gui_synthesizer_circuit::reset_content()
		{
			std::deque<abstract_gui_component*> temp(m_widgets);

			for (abstract_gui_component * component : temp) {
				if (component->get_sound_component_factory_id() != Sound::NO_FACTORY )
					remove_widget(component);
			}
		}


	} /* Gui */

} /* Gammou */
