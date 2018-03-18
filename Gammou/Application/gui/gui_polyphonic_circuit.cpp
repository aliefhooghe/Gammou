
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

		abstract_gui_component * gui_polyphonic_circuit::gui_component_by_internal_id(const uint32_t internal_id)
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
			m_master_out = new internal_gui_component(
				m_synthesizer->get_polyphonic_circuit_master_output(),
				internal_component_id::MASTER_OUT,
				200, 10);

			m_master_in = new internal_gui_component(
				m_synthesizer->get_polyphonic_circuit_master_input(),
				internal_component_id::MASTER_IN,
				10, 10);

			m_midi_input = new internal_gui_component(
				m_synthesizer->get_polyphonic_circuit_midi_input(),
				internal_component_id::MIDI,
				10, 200);

			m_parameter_input = new internal_gui_component(
				m_synthesizer->get_polyphonic_circuit_parameter_input(),
				internal_component_id::PARAMETERS,
				400, 10);

			add_gui_component(m_master_in);
			add_gui_component(m_master_out);
			add_gui_component(m_midi_input);
			add_gui_component(m_parameter_input);
		}

	} /* Gui */
}