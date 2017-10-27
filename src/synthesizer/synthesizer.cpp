#include <numeric>
#include "synthesizer.h"
#include<iostream>

namespace gammou {



/*
 *
 */

synthesizer::synthesizer(const unsigned int main_input_count,
							const unsigned int main_output_count,
							const unsigned int channel_count,
							const unsigned int automation_count,
							const unsigned int master_to_polyphonic_count,
							const unsigned int polyphonic_to_master_count,
							const unsigned int channel_zero_lifetime)
	: m_automation_input_buffer(automation_count),
	  m_master_to_polyphonic_buffer(master_to_polyphonic_count),
	  m_polyphonic_to_master_buffer(polyphonic_to_master_count),
	  m_polyphonic_circuit(channel_count,
			  	  	  	master_to_polyphonic_count,
			  	  	  	automation_count,
			  	  	  	polyphonic_to_master_count,
			  	  	  	m_polyphonic_to_master_buffer.data(),
			  	  	  	m_automation_input_buffer.data(),
			  	  	  	m_master_to_polyphonic_buffer.data()),
	 m_master_circuit(master_to_polyphonic_count,
			 automation_count, main_input_count,
			 main_output_count, polyphonic_to_master_count,
			 m_master_to_polyphonic_buffer.data(),
			 m_polyphonic_to_master_buffer.data(),
			 m_automation_input_buffer.data()),
	m_channels(channel_count),
	m_running_channels_end(m_channels.begin()),
	m_channels_lifetime(channel_count),
	m_channel_zero_lifetime(channel_zero_lifetime),
	m_channels_midi_note(channel_count)
{
	std::iota(m_channels.begin(), m_channels.end(), 0u);
}

synthesizer::~synthesizer()
{

}

void synthesizer::process(const double input[], double output[])
{
	std::fill(m_polyphonic_to_master_buffer.begin(),
			m_polyphonic_to_master_buffer.end(), 0.0);

	DEBUG_PRINT("[");

	for(auto it = m_channels.begin(); it != m_running_channels_end; ){
		const unsigned int current_channel = *it;


		DEBUG_PRINT(",%d", m_channels_lifetime[current_channel]);

		if( m_polyphonic_circuit.process(current_channel)){
			// Output == Zero for this channel

			DEBUG_PRINT("--");

			if( (--(m_channels_lifetime[current_channel])) == 0u){
				// no more lifetime
				//DEBUG_PRINT("free:%d\n", current_channel);
				free_channel(it);
				continue;
			}
		}
		else{
			DEBUG_PRINT("#", current_channel);
			m_channels_lifetime[current_channel] = m_channel_zero_lifetime;
		}

		++it;
	}

	DEBUG_PRINT("]\n");

	m_master_circuit.process(input, output);
}


void synthesizer::send_note_on(const char midi_note, const double velocity)
{
	const unsigned int channel = get_new_channel();

	if( channel != INVALID_CHANNEL ){

		//DEBUG_PRINT("on : %d\n", channel);

		m_channels_midi_note[channel] = midi_note;
		m_polyphonic_circuit.initialize_channel(channel);
		m_polyphonic_circuit.set_channel_pitch(channel, m_note_frequencies[(int)midi_note]);
		m_polyphonic_circuit.set_channel_attack_velocity(channel, velocity);
		m_polyphonic_circuit.set_channel_gate_state(channel, true);
		// default value (avoid undetermined component behavior)
		m_polyphonic_circuit.set_channel_release_velocity(channel, 1.0);

		m_channels_lifetime[channel] = m_channel_zero_lifetime;
	}

	// Else do nothing
}

void synthesizer::send_note_off(char midi_note, const double velocity)
{
	for(auto it = m_channels.begin(); it != m_running_channels_end; ++it){
		const unsigned int channel = *it;

		if( m_channels_midi_note[channel] == midi_note ) {
			//DEBUG_PRINT("off: %d\n", channel);
			m_channels_midi_note[channel] = NO_NOTE;
			m_polyphonic_circuit.set_channel_release_velocity(channel, velocity);
			m_polyphonic_circuit.set_channel_gate_state(channel, false);
			break;
		}
	}


}

void synthesizer::add_sound_component_on_master_circuit(abstract_sound_component *component)
{
	m_master_circuit.add_sound_component(component);
}

void synthesizer::add_sound_component_on_polyphonic_circuit(abstract_sound_component *component)
{
	m_polyphonic_circuit.add_sound_component(component);
}

// polyphonic_circuit_ component

process::abstract_component<double> *synthesizer::get_polyphonic_circuit_gpar_input()
{
	return m_polyphonic_circuit.get_gpar_input();
}

process::abstract_component<double> *synthesizer::get_polyphonic_circuit_master_input()
{
	return m_polyphonic_circuit.get_master_input();
}

process::abstract_component<double> *synthesizer::get_polyphonic_circuit_automation_input()
{
	return m_polyphonic_circuit.get_automation_input();
}

process::abstract_component<double> *synthesizer::get_polyphonic_circuit_output()
{
	return m_polyphonic_circuit.get_output();
}

// master_circuit_ component

process::abstract_component<double> *synthesizer::get_master_main_input()
{
	return m_master_circuit.get_main_input();
}

process::abstract_component<double> *synthesizer::get_master_main_output()
{
	return m_master_circuit.get_main_output();
}

process::abstract_component<double> *synthesizer::get_master_circuit_polyphonic_input()
{
	return m_master_circuit.get_polyphonic_input();
}

process::abstract_component<double> *synthesizer::get_master_circuit_polyphonic_output()
{
	return m_master_circuit.get_polyphonic_output();
}

process::abstract_component<double> *synthesizer::get_master_circuit_automation_input()
{
	return m_master_circuit.get_automation_input();
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




const double synthesizer::m_note_frequencies[127] = {
		 8.1757989156,
		 8.661957218,
		 9.1770239974,
		 9.7227182413,
		 10.3008611535,
		 10.9133822323,
		 11.5623257097,
		 12.2498573744,
		 12.9782717994,
		 13.75,
		 14.5676175474,
		 15.4338531643,
		 16.3515978313,
		 17.3239144361,
		 18.3540479948,
		 19.4454364826,
		 20.6017223071,
		 21.8267644646,
		 23.1246514195,
		 24.4997147489,
		 25.9565435987,
		 27.5,
		 29.1352350949,
		 30.8677063285,
		 32.7031956626,
		 34.6478288721,
		 36.7080959897,
		 38.8908729653,
		 41.2034446141,
		 43.6535289291,
		 46.249302839,
		 48.9994294977,
		 51.9130871975,
		 55,
		 58.2704701898,
		 61.735412657,
		 65.4063913251,
		 69.2956577442,
		 73.4161919794,
		 77.7817459305,
		 82.4068892282,
		 87.3070578583,
		 92.4986056779,
		 97.9988589954,
		 103.826174395,
		 110,
		 116.5409403795,
		 123.470825314,
		 130.8127826503,
		 138.5913154884,
		 146.8323839587,
		 155.563491861,
		 164.8137784564,
		 174.6141157165,
		 184.9972113558,
		 195.9977179909,
		 207.65234879,
		 220,
		 233.081880759,
		 246.9416506281,
		 261.6255653006,
		 277.1826309769,
		 293.6647679174,
		 311.1269837221,
		 329.6275569129,
		 349.228231433,
		 369.9944227116,
		 391.9954359818,
		 415.3046975799,
		 440,
		 466.1637615181,
		 493.8833012561,
		 523.2511306012,
		 554.3652619537,
		 587.3295358348,
		 622.2539674442,
		 659.2551138257,
		 698.456462866,
		 739.9888454233,
		 783.9908719635,
		 830.6093951599,
		 880,
		 932.3275230362,
		 987.7666025122,
		 1046.5022612024,
		 1108.7305239075,
		 1174.6590716696,
		 1244.5079348883,
		 1318.5102276515,
		 1396.912925732,
		 1479.9776908465,
		 1567.981743927,
		 1661.2187903198,
		 1760,
		 1864.6550460724,
		 1975.5332050245,
		 2093.0045224048,
		 2217.461047815,
		 2349.3181433393,
		 2489.0158697767,
		 2637.020455303,
		 2793.825851464,
		 2959.9553816931,
		 3135.963487854,
		 3322.4375806396,
		 3520,
		 3729.3100921447,
		 3951.066410049,
		 4186.0090448096,
		 4434.92209563,
		 4698.6362866785,
		 4978.0317395533,
		 5274.0409106059,
		 5587.6517029281,
		 5919.9107633862,
		 6271.926975708,
		 6644.8751612791,
		 7040,
		 7458.6201842894,
		 7902.132820098,
		 8372.0180896192,
		 8869.8441912599,
		 9397.2725733571,
		 9956.0634791066,
		 10548.0818212118,
		 11175.3034058561};


}  // namespace gammou
