
#include "gui_polyphonic_circuit.h"

namespace Gammou {

	namespace Gui {

		/*
		gui polyphonic circuit implementation
		*/

		gui_polyphonic_circuit::gui_polyphonic_circuit(
			gui_component_main_factory *complete_component_factory,
			Sound::synthesizer * synthesizer,
			std::mutex * synthesizer_mutex, unsigned int x,
			const unsigned int y, const unsigned int width, const unsigned height, const View::color background)
			: abstract_gui_synthesizer_circuit(
				complete_component_factory, synthesizer->get_channel_count(), synthesizer,
				synthesizer_mutex, x, y, width, height, background)
		{
			add_internal_components(synthesizer_mutex);
		}

		gui_polyphonic_circuit::gui_polyphonic_circuit(
			gui_component_main_factory *complete_component_factory,
			Sound::synthesizer * synthesizer,
			std::mutex * synthesizer_mutex,
			const View::rectangle & rect, const View::color background)
			: abstract_gui_synthesizer_circuit(
				complete_component_factory, synthesizer->get_channel_count(), synthesizer,
				synthesizer_mutex, rect, background)
		{
			add_internal_components(synthesizer_mutex);
		}

		void gui_polyphonic_circuit::add_sound_component_to_frame(Sound::abstract_sound_component * sound_component)
		{
			m_synthesizer->add_sound_component_on_polyphonic_circuit(sound_component);
		}

		uint8_t gui_polyphonic_circuit::get_component_internal_id(abstract_gui_component * component)
		{
			const Process::abstract_component<double> *const process_component = component->get_component();

			if (process_component == m_synthesizer->get_polyphonic_circuit_master_output())
				return internal_component_id::MASTER_OUT;
			else if (process_component == m_synthesizer->get_polyphonic_circuit_master_input())
				return internal_component_id::MASTER_IN;
			else if (process_component == m_synthesizer->get_polyphonic_circuit_midi_input())
				return internal_component_id::MIDI;
			else if (process_component == m_synthesizer->get_polyphonic_circuit_parameter_input())
				return internal_component_id::PARAMETERS;
			else
				throw std::domain_error("Component is not master Internal\n");
		}

		abstract_gui_component * gui_polyphonic_circuit::gui_component_by_internal_id(const uint8_t internal_id)
		{
			
			switch (internal_id)
			{

				case internal_component_id::MASTER_OUT:
					return m_master_out;
					break;

				case internal_component_id::MASTER_IN:
					return m_master_in;
					break;

				case internal_component_id::MIDI:
					return m_midi_input;
					break;

				case internal_component_id::PARAMETERS:
					return m_parameter_input;
					break;

				default:
					throw std::domain_error("Invalid Internal ID\n");
					break;
			}

		}

		void gui_polyphonic_circuit::add_internal_components(std::mutex * synthesizer_mutex)
		{
			m_master_out = new default_gui_component(
				m_synthesizer->get_polyphonic_circuit_master_output(),
				/*synthesizer_mutex,*/ 200, 10);

			m_master_in = new default_gui_component(
				m_synthesizer->get_polyphonic_circuit_master_input(),
				/*synthesizer_mutex,*/ 10, 10);

			m_midi_input = new default_gui_component(
				m_synthesizer->get_polyphonic_circuit_midi_input(),
				/*synthesizer_mutex,*/ 200, 10);

			m_parameter_input = new default_gui_component(
				m_synthesizer->get_polyphonic_circuit_parameter_input(),
				/*synthesizer_mutex,*/ 400, 10);

			add_gui_component(m_master_in);
			add_gui_component(m_master_out);
			add_gui_component(m_midi_input);
			add_gui_component(m_parameter_input);
		}

	} /* Gui */
}