#include <numeric>
#include <iostream>

#include "synthesizer.h"
#include "../debug.h"

namespace Gammou {

	namespace Sound {

		/*
		 *
		 */

		synthesizer::synthesizer(
			Process::abstract_frame_processor<double>& master_circuit_processor,
			Process::abstract_frame_processor<double>& polyphonic_circuit_processor,
			const unsigned int main_input_count,
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
				polyphonic_to_master_count,
				master_circuit_processor),
			m_polyphonic_circuit(
				&m_master_circuit, 
				channel_count,
				polyphonic_circuit_processor),
			m_channels(channel_count),
			m_running_channels_end(m_channels.begin()),
			m_channels_lifetime(channel_count),
            m_channel_zero_lifetime(static_cast<double>(channel_zero_lifetime)),
            m_channels_midi_note(channel_count),
            m_keyboard_mode(keyboard_mode::POLYPHONIC)
		{
			DEBUG_PRINT("Synthesizer CTOR\n");
			set_sample_rate(sample_rate);
			std::iota(m_channels.begin(), m_channels.end(), 0u);
		}

		synthesizer::~synthesizer()
		{
			DEBUG_PRINT("Synthesizer DTOR\n");
		}

		void synthesizer::process(const double input[], double output[])
		{
			std::fill(m_master_circuit.m_polyphonic_to_master_buffer.begin(),
				m_master_circuit.m_polyphonic_to_master_buffer.end(), 0.0);

			for (auto it = m_channels.begin(); it != m_running_channels_end; ){
				const unsigned int current_channel = *it;

				if( m_polyphonic_circuit.process(current_channel)){
					// IF Output == Zero for this channel

					if( (--(m_channels_lifetime[current_channel])) == 0u){
						// no more lifetime
						free_channel(it);
						DEBUG_PRINT("freed channel %d\n", current_channel);
						continue;
					}

				}
				else{
					// tODO BETTER
					m_channels_lifetime[current_channel] = m_channel_zero_sample_count;
				}

				++it;
			}

			m_master_circuit.process(input, output);

		}

		void synthesizer::set_master_volume(const double volume_order)
		{
			m_master_circuit.set_volume(volume_order);
		}

		void synthesizer::send_note_on(const unsigned int midi_note, const double velocity)
		{
            unsigned int channel;

			if (m_keyboard_mode == keyboard_mode::POLYPHONIC ||
				get_running_channel_count() == 0) {
				channel = get_new_channel();
				m_polyphonic_circuit.initialize_channel(channel);
			}
			else {
				channel = m_channels[0];
			}

            if( channel != INVALID_CHANNEL ){

                DEBUG_PRINT("on : channel = %d, note = %d, fr = %f)\n", channel, midi_note, m_note_frequencies[midi_note]);

                m_channels_midi_note[channel] = midi_note;
              
                m_polyphonic_circuit.set_channel_pitch(channel, m_note_frequencies[midi_note]);
                m_polyphonic_circuit.set_channel_attack_velocity(channel, velocity);
                m_polyphonic_circuit.set_channel_gate_state(channel, true);

                // default reelase value (avoid undetermined component behavior)
                m_polyphonic_circuit.set_channel_release_velocity(channel, 0.5);

                m_channels_lifetime[channel] = m_channel_zero_sample_count;
            }
		}

		void synthesizer::send_note_off(const unsigned int midi_note, const double velocity)
		{
            //  TODO use map
			for(auto it = m_channels.begin(); it != m_running_channels_end; ++it){
				const unsigned int channel = *it;

				if( m_channels_midi_note[channel] == midi_note ) {
					//DEBUG_PRINT("off: %d\n", channel);
					m_channels_midi_note[channel] = NO_NOTE;
					m_polyphonic_circuit.set_channel_release_velocity(channel, velocity);
					m_polyphonic_circuit.set_channel_gate_state(channel, false);
					return;
				}
			}

			DEBUG_PRINT("off Inaplicable : (n = %d)\n", midi_note);
		}

        void synthesizer::set_keyboard_mode(const keyboard_mode mode)
        {
            m_keyboard_mode = mode;
        }

        synthesizer::keyboard_mode synthesizer::get_keyboard_mode() const
        {
            return m_keyboard_mode;
        }

		void synthesizer::add_sound_component_on_master_circuit(abstract_sound_component *component)
		{
			// Component process initialisation on channel 0 done by master circuit
			m_master_circuit.add_sound_component(component);
		}

		void synthesizer::add_sound_component_on_polyphonic_circuit(abstract_sound_component *component)
		{
			if (component->get_channel_count() != get_channel_count())
				throw std::domain_error("Component's channel count does not fit");

			//  Initialize component for each running channel

			for (auto it = m_channels.begin(); it != m_running_channels_end; ++it) {
				const unsigned int current_channel = *it;
				component->set_working_channel_ref(&current_channel);
				component->initialize_process();
			}

			//	--
			m_polyphonic_circuit.add_sound_component(component);		
		}

		void synthesizer::set_sample_rate(const double sample_rate)
		{
			DEBUG_PRINT("Synthesizer Set Sample Rate %lf\n", sample_rate);
			m_channel_zero_sample_count = (static_cast<unsigned int>(sample_rate * m_channel_zero_lifetime));
			m_master_circuit.set_sample_rate(sample_rate);
			m_polyphonic_circuit.set_sample_rate(sample_rate);
		}

		// polyphonic_circuit_ component

		Process::abstract_component<double> *synthesizer::get_polyphonic_circuit_midi_input()
		{
			return &(m_polyphonic_circuit.m_midi_input);
		}

		Process::abstract_component<double> *synthesizer::get_polyphonic_circuit_master_output()
		{
			return &(m_polyphonic_circuit.m_master_output);
		}

		Process::abstract_component<double> *synthesizer::get_polyphonic_circuit_parameter_input()
		{
			return &(m_polyphonic_circuit.m_parameter_input);
		}

		Process::abstract_component<double> *synthesizer::get_polyphonic_circuit_master_input()
		{
			return &(m_polyphonic_circuit.m_master_input);
		}

		// master_circuit_ component

		Process::abstract_component<double> *synthesizer::get_master_main_input()
		{
			return &(m_master_circuit.m_main_input);
		}

		Process::abstract_component<double> *synthesizer::get_master_main_output()
		{
			return &(m_master_circuit.m_main_output);
		}

		Process::abstract_component<double> *synthesizer::get_master_circuit_polyphonic_output()
		{
			return &(m_master_circuit.m_polyphonic_output);
		}

		Process::abstract_component<double> *synthesizer::get_master_circuit_polyphonic_input()
		{
			return &(m_master_circuit.m_polyphonic_input);
		}

		Process::abstract_component<double> *synthesizer::get_master_circuit_parameter_input()
		{
			return &(m_master_circuit.m_parameter_input);
		}

		unsigned int synthesizer::get_parameter_input_count() const
		{
			return static_cast<unsigned int>(m_master_circuit.m_parameter_buffer.size());
		}

		void synthesizer::set_parameter_value(const double value, const unsigned int automation_id)
		{
			// TODO safer ???
			m_master_circuit.m_parameter_buffer[automation_id] = value;
		}

        double synthesizer::get_parameter_value(const unsigned int automation_id) const
		{
			return m_master_circuit.m_parameter_buffer[automation_id];
		}

		unsigned int synthesizer::get_channel_count() const
		{
			return static_cast<unsigned int>(m_channels.size());
		}

		unsigned int synthesizer::get_new_channel()
		{
			if( m_running_channels_end != m_channels.end())
				return *(m_running_channels_end++);
			else
				return INVALID_CHANNEL;
		}

        unsigned int synthesizer::get_running_channel_count()
        {
            return
                static_cast<unsigned int>(
                        m_running_channels_end - m_channels.begin());
        }

		void synthesizer::free_channel(const std::vector<unsigned int>::iterator& it)
		{
			std::iter_swap(it, m_running_channels_end - 1);
			m_running_channels_end--;
		}

		const double synthesizer::m_note_frequencies[128] =
		{
			8.176,
			8.662,
			9.177,
			9.723,
			10.301,
			10.913,
			11.562,
			12.250,
			12.978,
			13.750,
			14.568,
			15.434,
			16.352,
			17.324,
			18.354,
			19.445,
			20.602,
			21.827,
			23.125,
			24.500,
			25.957,
			27.500,
			29.135,
			30.868,
			32.703,
			34.648,
			36.708,
			38.891,
			41.203,
			43.654,
			46.249,
			48.999,
			51.913,
			55.000,
			58.270,
			61.735,
			65.406,
			69.296,
			73.416,
			77.782,
			82.407,
			87.307,
			92.499,
			97.999,
			103.826,
			110.000,
			116.541,
			123.471,
			130.813,
			138.591,
			146.832,
			155.563,
			164.814,
			174.614,
			184.997,
			195.998,
			207.652,
			220.000,
			233.082,
			246.942,
			261.626,
			277.183,
			293.665,
			311.127,
			329.628,
			349.228,
			369.994,
			391.995,
			415.305,
			440.000,
			466.164,
			493.883,
			523.251,
			554.365,
			587.330,
			622.254,
			659.255,
			698.456,
			739.989,
			783.991,
			830.609,
			880.000,
			932.328,
			987.767,
			1046.502,
			1108.731,
			1174.659,
			1244.508,
			1318.510,
			1396.913,
			1479.978,
			1567.982,
			1661.219,
			1760.000,
			1864.655,
			1975.533,
			2093.005,
			2217.461,
			2349.318,
			2489.016,
			2637.020,
			2793.826,
			2959.955,
			3135.963,
			3322.438,
			3520.000,
			3729.310,
			3951.066,
			4186.009,
			4434.922,
			4698.636,
			4978.032,
			5274.041,
			5587.652,
			5919.911,
			6271.927,
			6644.875,
			7040.000,
			7458.620,
			7902.133,
			8372.018,
			8869.844,
			9397.273,
			9956.063,
			10548.080,
			11175.300,
			11839.820,
			12543.850 };

	} /* Sound */

}  // namespace Gammou
