#include <numeric>
#include <iostream>

#include "synthesizer.h"
#include "../debug.h"

namespace Gammou {

	namespace Sound {

		/*
		*
		*/

		synthesizer::synthesizer(const unsigned int main_input_count,
									const unsigned int main_output_count,
									const unsigned int channel_count,
									const unsigned int automation_count,
									const unsigned int master_to_polyphonic_count,
									const unsigned int polyphonic_to_master_count,
									const unsigned int sample_rate,
									const float channel_zero_lifetime)
			:
			m_master_circuit(
				master_to_polyphonic_count, 
				automation_count, main_input_count,
				main_output_count, 
				polyphonic_to_master_count),
			m_polyphonic_circuit(&m_master_circuit, channel_count),
			m_channels(channel_count),
			m_running_channels_end(m_channels.begin()),
			m_channels_lifetime(channel_count),
			m_channel_zero_lifetime(static_cast<unsigned int>(sample_rate * channel_zero_lifetime)),
			m_channels_midi_note(channel_count)
		{
			m_master_circuit.set_sample_rate(sample_rate);
			m_polyphonic_circuit.set_sample_rate(sample_rate);
			std::iota(m_channels.begin(), m_channels.end(), 0u);
		}

		synthesizer::~synthesizer()
		{

		}

		void synthesizer::process(const double input[], double output[])
		{
			std::fill(m_master_circuit.m_polyphonic_to_master_buffer.begin(),
				m_master_circuit.m_polyphonic_to_master_buffer.end(), 0.0);

			for(auto it = m_channels.begin(); it != m_running_channels_end; ){
				const unsigned int current_channel = *it;

				if( m_polyphonic_circuit.process(current_channel)){
					// Output == Zero for this channel

					if( (--(m_channels_lifetime[current_channel])) == 0u){
						// no more lifetime
						DEBUG_PRINT("free:%d\n", current_channel);
						free_channel(it);
						continue;
					}

				}
				else{
					m_channels_lifetime[current_channel] = m_channel_zero_lifetime;
				}

				++it;
			}

			m_master_circuit.process(input, output);
		}


		void synthesizer::send_note_on(const char midi_note, const double velocity)
		{
			const unsigned int channel = get_new_channel();

			if( channel != INVALID_CHANNEL ){

				DEBUG_PRINT("on : %d (n = %d), fr = %f)\n", channel, midi_note, m_note_frequencies[(int)midi_note]);

				m_channels_midi_note[channel] = midi_note;
				m_polyphonic_circuit.initialize_channel(channel);
				m_polyphonic_circuit.set_channel_pitch(channel, m_note_frequencies[(int)midi_note]);
				m_polyphonic_circuit.set_channel_attack_velocity(channel, velocity);
				m_polyphonic_circuit.set_channel_gate_state(channel, true);
				// default value (avoid undetermined component behavior)
				m_polyphonic_circuit.set_channel_release_velocity(channel, 0.5);

				m_channels_lifetime[channel] = m_channel_zero_lifetime;
			}
			else {
				DEBUG_PRINT("No more channel\n");
			}

			// Else do nothing
		}

		void synthesizer::send_note_off(char midi_note, const double velocity)
		{
			for(auto it = m_channels.begin(); it != m_running_channels_end; ++it){
				const unsigned int channel = *it;

				if( m_channels_midi_note[channel] == midi_note ) {
					DEBUG_PRINT("off: %d\n", channel);
					m_channels_midi_note[channel] = NO_NOTE;
					m_polyphonic_circuit.set_channel_release_velocity(channel, velocity);
					m_polyphonic_circuit.set_channel_gate_state(channel, false);
					return;
				}
			}

			DEBUG_PRINT("off Inaplicable : (n = %d)\n", midi_note);
		}

		void synthesizer::add_sound_component_on_master_circuit(abstract_sound_component *component)
		{
			// Component process initialisation on channel 0 done by master circuit
			m_master_circuit.add_sound_component(component);
		}

		void synthesizer::add_sound_component_on_polyphonic_circuit(polyphonic_sound_component *component)
		{
			if (component->get_channel_count() != get_channel_count())
				throw std::domain_error("Component's channel count does not fit");

			//  Initialize component for each running channel
			for (auto it = m_channels.begin(); it != m_running_channels_end; ) {
				const unsigned int current_channel = *it;
				component->on_channel_change(current_channel);
				component->initialize_process();
			}

			m_polyphonic_circuit.add_sound_component(component);
		}

		void synthesizer::set_sample_rate(const double sample_rate)
		{
			m_master_circuit.set_sample_rate(sample_rate);
			m_polyphonic_circuit.set_sample_rate(sample_rate);
		}

		// polyphonic_circuit_ component

		Process::abstract_component<double> *synthesizer::get_polyphonic_circuit_gpar_input()
		{
			return &(m_polyphonic_circuit.m_gpar_input);
		}

		Process::abstract_component<double> *synthesizer::get_polyphonic_circuit_master_input()
		{
			return &(m_polyphonic_circuit.m_input_from_master);
		}

		Process::abstract_component<double> *synthesizer::get_polyphonic_circuit_automation_input()
		{
			return &(m_polyphonic_circuit.m_automation_input);
		}

		Process::abstract_component<double> *synthesizer::get_polyphonic_circuit_output()
		{
			return &(m_polyphonic_circuit.m_output_to_master);
		}

		// master_circuit_ component

		Process::abstract_component<double> *synthesizer::get_master_main_input()
		{
			return &(m_master_circuit.m_main_input);
		}

		Process::abstract_component<double> *synthesizer::get_master_main_output()
		{
			return  &(m_master_circuit.m_main_output);
		}

		Process::abstract_component<double> *synthesizer::get_master_circuit_polyphonic_input()
		{
			return &(m_master_circuit.m_from_polyphonic_input);
		}

		Process::abstract_component<double> *synthesizer::get_master_circuit_polyphonic_output()
		{
			return &(m_master_circuit.m_output_to_polyphonic);
		}

		Process::abstract_component<double> *synthesizer::get_master_circuit_automation_input()
		{
			return &(m_master_circuit.m_automation_input);
		}

		unsigned int synthesizer::get_automation_input_count() const
		{
			return m_master_circuit.m_automation_buffer.size();
		}

		void synthesizer::set_automation_value(const double value, const unsigned int automation_id)
		{
			// TODO safer ???
			m_master_circuit.m_automation_buffer[automation_id] = value;
		}

		unsigned int synthesizer::get_channel_count() const
		{
			return m_channels.size();
		}


		unsigned int synthesizer::get_new_channel()
		{
			if( m_running_channels_end != m_channels.end())
				return *(m_running_channels_end++);
			else
				return INVALID_CHANNEL;
		}

		void synthesizer::free_channel(const std::vector<unsigned int>::iterator& it)
		{
			std::iter_swap(it, m_running_channels_end - 1);
			m_running_channels_end--;
		}




		const double synthesizer::m_note_frequencies[127] = { // todo mieux
			16.3,
			17.3,
			18.3,
			19.4,
			20.5,
			21.8,
			23.1,
			24.5,
			26.0,
			27.5,
			29.1,
			30.8,
			32.7,
			34.6,
			36.7,
			38.9,
			41.2,
			43.6,
			46.2,
			49.0,
			51.9,
			55.0,
			58.0,
			62.0,
			65,
			69,
			74,
			78,
			83,
			87,
			92.5,
			98,
			104,
			110,
			117,
			123,
			131,
			139,
			147,
			156,
			165,
			175,
			185,
			196,
			208,
			220,
			233,
			247,
			262,
			277,
			294,
			311,
			330,
			349,
			370,
			392,
			415,
			440,
			466,
			494,
			523,
			554,
			587,
			622,
			659,
			698.5,
			740,
			784,
			831,
			880,
			932,
			988,
			1046.5,
			1109,
			1175,
			1244.5,
			1318.5,
			1397,
			1480,
			1568,
			1661,
			1760,
			1865,
			1975,
			2093,
			2217,
			2349,
			2489,
			2637,
			2794,
			2960,
			3136,
			3322,
			3520,
			3729,
			3951,
			4186,
			4435,
			4698,
			4978,
			5274,
			5588,
			5920,
			6272,
			6645,
			7040,
			7458,
			7902,
			8372,
			8870,
			9396,
			9956,
			10548,
			11176,
			11840,
			12544,
			13290,
			14080,
			14918,
			15804,
			16744,
			17740,
			18792,
			19912,
			21098,
			21098};

	} /* Sound */

}  // namespace Gammou
