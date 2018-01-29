
#include "gui_synthesizer_circuit.h"

namespace Gammou {

	namespace Gui {

		/*
				abstract gui synthesizer circuit implementation
		*/

		abstract_gui_synthesizer_circuit::abstract_gui_synthesizer_circuit(
			Sound::main_factory *main_factory,
			Sound::synthesizer * synthesizer, 
			std::mutex * synthesizer_mutex, 
			unsigned int x, const unsigned int y, const unsigned int width, const unsigned int height, const View::color background)
			: abstract_gui_component_map(synthesizer_mutex, x, y, width, height, background),
			m_synthesizer(synthesizer),
			m_synthesizer_mutex(synthesizer_mutex),
			m_main_factory(main_factory),
			m_creation_factory_id(Sound::NO_FACTORY)
		{
		}

		abstract_gui_synthesizer_circuit::abstract_gui_synthesizer_circuit(
			Sound::main_factory *main_factory,
			Sound::synthesizer * synthesizer,
			std::mutex * synthesizer_mutex, const View::rectangle & rect, const View::color background)
			: abstract_gui_component_map(synthesizer_mutex, rect, background),
			m_synthesizer(synthesizer),
			m_synthesizer_mutex(synthesizer_mutex),
			m_main_factory(main_factory),
			m_creation_factory_id(Sound::NO_FACTORY)
		{
		}

		bool abstract_gui_synthesizer_circuit::on_mouse_dbl_click(const int x, const int y)
		{

			if (!abstract_gui_component_map::on_mouse_dbl_click(x, y) 
				&& m_creation_factory_id != Sound::NO_FACTORY) {

				DEBUG_PRINT("Create Componnent\n");

				const Sound::request_form& requests = m_main_factory->get_plugin_request_form(m_creation_factory_id);
				
				if (requests.get_request_count() != 0) // TODO Handle requests
					throw std::runtime_error("Factory Request not handled!");
			
				const Sound::answer_form answers;

				Sound::abstract_sound_component *sound_component = 
					m_main_factory->get_new_sound_component(m_creation_factory_id, answers, m_synthesizer->get_channel_count());

				gui_sound_component *gui_component =
					new gui_sound_component(sound_component, m_synthesizer_mutex, x, y);

				lock_circuit();
				add_sound_component_to_frame(sound_component);
				unlock_circuit();

				add_gui_component(gui_component);	
			}

			return true;
		}

		void abstract_gui_synthesizer_circuit::select_component_creation_factory_id(const unsigned int factory_id)
		{
			DEBUG_PRINT("Set Factory Id = %u\n", factory_id);

			if (m_main_factory->check_factory_presence(factory_id))
				m_creation_factory_id = factory_id;
			else
				DEBUG_PRINT("Unregisterd Factory");
		}

		/*
			gui master circuit implementation
		*/

		gui_master_circuit::gui_master_circuit(
			Sound::main_factory *main_factory, 
			Sound::synthesizer * synthesizer, 
			std::mutex *synthesizer_mutex,
			const unsigned int x, const unsigned int y, const unsigned int width, const unsigned int height, const View::color background)
			: abstract_gui_synthesizer_circuit(main_factory, synthesizer, synthesizer_mutex, x, y, width, height, background)
		{
			add_internal_components(synthesizer_mutex);
		}

		gui_master_circuit::gui_master_circuit(
			Sound::main_factory *main_factory, 
			Sound::synthesizer * synthesizer, 
			std::mutex *synthesizer_mutex,
			const View::rectangle & rect, const View::color background)
			: abstract_gui_synthesizer_circuit(main_factory, synthesizer, synthesizer_mutex, rect, background)
		{
			add_internal_components(synthesizer_mutex);
		}

		void gui_master_circuit::add_sound_component_to_frame(Sound::abstract_sound_component * sound_component)
		{
			m_synthesizer->add_sound_component_on_master_circuit(sound_component);
		}

		void gui_master_circuit::add_internal_components(std::mutex *synthesizer_mutex)
		{
			// Todo position
			add_gui_component(
				new default_gui_component(
					m_synthesizer->get_master_circuit_automation_input(), 
					synthesizer_mutex, 50, 50));
			add_gui_component(
				new default_gui_component(
					m_synthesizer->get_master_circuit_polyphonic_input(),
					synthesizer_mutex, 60, 60));
			add_gui_component(
				new default_gui_component(
					m_synthesizer->get_master_circuit_polyphonic_output() ,
					synthesizer_mutex, 70, 70));
			add_gui_component(
				new default_gui_component(
					m_synthesizer->get_master_main_input() ,
					synthesizer_mutex, 80, 80));
			add_gui_component(
				new default_gui_component(
					m_synthesizer->get_master_main_output() ,
					synthesizer_mutex, 90, 90));

		//	for(unsigned int i = 0; i < 2 ; ++i)
		//		m_synthesizer->get_master_circuit_polyphonic_input()->connect_to(
		//			i,
		//			m_synthesizer->get_master_main_output(),
		//			i
		//	);
		}

		/*
			gui polyphonic circuit implementation
		*/

		gui_polyphonic_circuit::gui_polyphonic_circuit(
			Sound::main_factory *main_factory,
			Sound::synthesizer * synthesizer, 
			std::mutex * synthesizer_mutex, unsigned int x, 
			const unsigned int y, const unsigned int width, const unsigned height, const View::color background)
			: abstract_gui_synthesizer_circuit(main_factory, synthesizer, synthesizer_mutex, x, y, width, height, background)
		{
			add_internal_components(synthesizer_mutex);
		}

		gui_polyphonic_circuit::gui_polyphonic_circuit(
			Sound::main_factory *main_factory,
			Sound::synthesizer * synthesizer, 
			std::mutex * synthesizer_mutex,
			const View::rectangle & rect, const View::color background)
			: abstract_gui_synthesizer_circuit(main_factory, synthesizer, synthesizer_mutex, rect, background)
		{
			add_internal_components(synthesizer_mutex);
		}

		void gui_polyphonic_circuit::add_sound_component_to_frame(Sound::abstract_sound_component * sound_component)
		{
			m_synthesizer->add_sound_component_on_polyphonic_circuit(sound_component);
		}

		void gui_polyphonic_circuit::add_internal_components(std::mutex * synthesizer_mutex)
		{
			add_gui_component(
				new default_gui_component(
					m_synthesizer->get_polyphonic_circuit_automation_input(),
					synthesizer_mutex, 10, 10));
			add_gui_component(
				new default_gui_component(
					m_synthesizer->get_polyphonic_circuit_gpar_input(),
					synthesizer_mutex, 10, 10));
			add_gui_component(
				new default_gui_component(
					m_synthesizer->get_polyphonic_circuit_master_input(),
					synthesizer_mutex, 10, 10));
			add_gui_component(
				new default_gui_component(
					m_synthesizer->get_polyphonic_circuit_output(),
					synthesizer_mutex, 10, 10));
		}



} /* Gui */

} /* Gammou */
