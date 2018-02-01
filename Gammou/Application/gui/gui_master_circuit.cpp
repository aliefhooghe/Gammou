
#include "gui_master_circuit.h"


namespace Gammou {

	namespace Gui {

		/*
		gui master circuit implementation
		*/

		gui_master_circuit::gui_master_circuit(
			Sound::main_factory *main_factory,
			Sound::synthesizer * synthesizer,
			std::mutex *synthesizer_mutex,
			const unsigned int x, const unsigned int y, const unsigned int width, const unsigned int height, const View::color background)
			: abstract_gui_synthesizer_circuit(
				main_factory, 1u, synthesizer, synthesizer_mutex,
				x, y, width, height, background)
		{
			add_internal_components(synthesizer_mutex);
		}

		gui_master_circuit::gui_master_circuit(
			Sound::main_factory *main_factory,
			Sound::synthesizer * synthesizer,
			std::mutex *synthesizer_mutex,
			const View::rectangle & rect, const View::color background)
			: abstract_gui_synthesizer_circuit(
				main_factory, 1u, synthesizer, synthesizer_mutex,
				rect, background)
		{
			add_internal_components(synthesizer_mutex);
		}

		void gui_master_circuit::add_sound_component_to_frame(Sound::abstract_sound_component * sound_component)
		{
			m_synthesizer->add_sound_component_on_master_circuit(sound_component);
		}

		uint8_t gui_master_circuit::get_component_internal_id(abstract_gui_component * component)
		{
			const Process::abstract_component<double> *const process_component = component->get_component();

			if (process_component == m_synthesizer->get_master_main_input())
				return internal_component_id::INPUT;
			else if (process_component == m_synthesizer->get_master_main_output())
				return internal_component_id::OUTPUT;
			else if (process_component == m_synthesizer->get_master_circuit_automation_input())
				return internal_component_id::PARAMETERS;
			else if (process_component == m_synthesizer->get_master_circuit_polyphonic_input())
				return internal_component_id::POLY_OUT;
			else if (process_component == m_synthesizer->get_master_circuit_polyphonic_output())
				return internal_component_id::POLY_IN;
			else
				throw std::domain_error("Component is not master Internal\n");
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
					m_synthesizer->get_master_circuit_polyphonic_output(),
					synthesizer_mutex, 70, 70));
			add_gui_component(
				new default_gui_component(
					m_synthesizer->get_master_main_input(),
					synthesizer_mutex, 80, 80));
			add_gui_component(
				new default_gui_component(
					m_synthesizer->get_master_main_output(),
					synthesizer_mutex, 90, 90));

			//	for(unsigned int i = 0; i < 2 ; ++i)
			//		m_synthesizer->get_master_circuit_polyphonic_input()->connect_to(
			//			i,
			//			m_synthesizer->get_master_main_output(),
			//			i
			//	);
		}

	} /* Gui */

} /* Gammou */