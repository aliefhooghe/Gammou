
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
			else if (process_component == m_synthesizer->get_master_circuit_parameter_input())
				return internal_component_id::PARAMETERS;
			else if (process_component == m_synthesizer->get_master_circuit_polyphonic_output())
				return internal_component_id::POLY_OUT;
			else if (process_component == m_synthesizer->get_master_circuit_polyphonic_input())
				return internal_component_id::POLY_IN;
			else
				throw std::domain_error("Component is not master Internal\n");
		}

		abstract_gui_component * gui_master_circuit::gui_component_by_internal_id(const uint8_t internal_id)
		{
			
			switch (internal_id)
			{

			case internal_component_id::INPUT:
				return m_main_input;
				break;

			case internal_component_id::OUTPUT:
				return m_main_output;
				break;

			case internal_component_id::PARAMETERS:
				return m_parameter_input;
				break;

			case internal_component_id::POLY_IN:
				return m_polyphonic_input;
				break;
			
			case internal_component_id::POLY_OUT:
				return m_polyphonic_output;
				break;

			default:
				throw std::domain_error("INVALID Internal Id\n");
				break;
			}

		}

		void gui_master_circuit::add_internal_components(std::mutex *synthesizer_mutex)
		{
			// Todo position

			m_polyphonic_input = new default_gui_component(
				m_synthesizer->get_master_circuit_polyphonic_input(),
				synthesizer_mutex, 50, 10);

			m_polyphonic_output = new default_gui_component(
				m_synthesizer->get_master_circuit_polyphonic_output(),
				synthesizer_mutex, 200, 10);

			m_main_input = new default_gui_component(
				m_synthesizer->get_master_main_input(),
				synthesizer_mutex, 50, 200);

			m_main_output = new default_gui_component(
				m_synthesizer->get_master_main_output(),
				synthesizer_mutex, 200, 200);

			m_parameter_input = new default_gui_component(
				m_synthesizer->get_master_circuit_parameter_input(),
				synthesizer_mutex, 50, 350);

			add_gui_component(m_parameter_input);
			add_gui_component(m_polyphonic_input);
			add_gui_component(m_polyphonic_output);
			add_gui_component(m_main_input);
			add_gui_component(m_main_output);
		}

	} /* Gui */

} /* Gammou */