
#include "gui_master_circuit.h"


namespace Gammou {

	namespace Gui {

		/*
		gui master circuit implementation
		*/

		gui_master_circuit::gui_master_circuit(
			gui_component_main_factory *complete_component_factory,
			Sound::synthesizer * synthesizer,
			std::mutex *synthesizer_mutex,
			const unsigned int x, const unsigned int y, const unsigned int width, const unsigned int height, const View::color background)
			: abstract_gui_synthesizer_circuit(
				complete_component_factory, 1u, synthesizer, synthesizer_mutex,
				x, y, width, height, background)
		{
			add_internal_components(synthesizer_mutex);
		}

		gui_master_circuit::gui_master_circuit(
			gui_component_main_factory *complete_component_factory,
			Sound::synthesizer * synthesizer,
			std::mutex *synthesizer_mutex,
			const View::rectangle & rect, const View::color background)
			: abstract_gui_synthesizer_circuit(
				complete_component_factory, 1u, synthesizer, synthesizer_mutex,
				rect, background)
		{
			add_internal_components(synthesizer_mutex);
		}

		void gui_master_circuit::add_sound_component_to_frame(Sound::abstract_sound_component * sound_component)
		{
			m_synthesizer->add_sound_component_on_master_circuit(sound_component);
		}

		abstract_gui_component * gui_master_circuit::gui_component_by_internal_id(const uint32_t internal_id)
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

			m_polyphonic_input = new internal_gui_component(
				m_synthesizer->get_master_circuit_polyphonic_input(),
				internal_component_id::POLY_IN,
				50, 10);

			m_polyphonic_output = new internal_gui_component(
				m_synthesizer->get_master_circuit_polyphonic_output(),
				internal_component_id::POLY_OUT,
				200, 10);

			m_main_input = new internal_gui_component(
				m_synthesizer->get_master_main_input(),
				internal_component_id::INPUT,
				50, 200);

			m_main_output = new internal_gui_component(
				m_synthesizer->get_master_main_output(),
				internal_component_id::OUTPUT,
				200, 200);

			m_parameter_input = new internal_gui_component(
				m_synthesizer->get_master_circuit_parameter_input(),
				internal_component_id::PARAMETERS,
				50, 350);

			add_gui_component(m_parameter_input);
			add_gui_component(m_polyphonic_input);
			add_gui_component(m_polyphonic_output);
			add_gui_component(m_main_input);
			add_gui_component(m_main_output);
		}

	} /* Gui */

} /* Gammou */