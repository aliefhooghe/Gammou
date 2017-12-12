
#include "polyphonic_circuit.h"

#include <limits>
#include <cmath>

namespace gammou{


polyphonic_circuit_GPAR_input::polyphonic_circuit_GPAR_input(const unsigned int channel_count)
	: abstract_sound_component("Input", 0, Input::COUNT, channel_count),
	  m_gate_state(channel_count),
	  m_pitch(channel_count),
	  m_attack_velocity(channel_count),
	  m_release_velocity(channel_count)
{
}

polyphonic_circuit_GPAR_input::~polyphonic_circuit_GPAR_input()
{
}

double polyphonic_circuit_GPAR_input::fetch_output(const unsigned int output_id)
{

	const unsigned int channel = get_current_working_channel();

	switch( output_id ) {

	case Input::GATE:
		return (double)(m_gate_state[channel]);
		break;

	case Input::PITCH:
		return m_pitch[channel];
		break;

	case Input::ATTACK_VELOCITY:
		return m_attack_velocity[channel];
		break;

	case Input::RELEASE_VELOCITY:
		return m_release_velocity[channel];
		break;
	}

	return 0.0;
}

void polyphonic_circuit_GPAR_input::set_channel_gate_state(const unsigned int channel, const bool state)
{
	m_gate_state[channel] = (state? 1.0 : 0.0);
}

void polyphonic_circuit_GPAR_input::set_channel_pitch(const unsigned int channel, const double pitch)
{
	m_pitch[channel] = pitch;
}

void polyphonic_circuit_GPAR_input::set_channel_attack_velocity(const unsigned int channel, const double velocity)
{
	m_attack_velocity[channel] = velocity;
}

void polyphonic_circuit_GPAR_input::set_channel_release_velocity(const unsigned int channel, const double velocity)
{
	m_release_velocity[channel] = velocity;
}


/*
 *
 */

polyphonic_circuit_output::polyphonic_circuit_output(const unsigned int output_count)
	: process::abstract_component<double>("Output", output_count, 0),
	  m_buffer_ptr(nullptr), m_last_out_was_zero(false)
{

}

void polyphonic_circuit_output::set_buffer_ptr(double buffer[])
{
	m_buffer_ptr = buffer;
}

void polyphonic_circuit_output::process(const double input[])
{
	m_last_out_was_zero = true;

	if( m_buffer_ptr != nullptr ){
		const unsigned int ic = get_input_count();

		for(unsigned int i = 0; i < ic; ++i){
			m_buffer_ptr[i] += input[i];

			if(std::abs(input[i]) > std::numeric_limits<float>::min())
				m_last_out_was_zero = false;
		}
	}

}

bool polyphonic_circuit_output::last_out_was_zero() const
{
	return m_last_out_was_zero;
}
/*
 *
 */


polyphonic_circuit::polyphonic_circuit(const unsigned int channel_count,
		const unsigned int master_to_polyphonic_output_count,
		const unsigned int automation_input_count,
		const unsigned int output_to_master_count,
		double output_to_master_buffer[],
		const double automation_buffer[],
		const double master_to_polyphonic_buffer[])
	: m_gpar_input(channel_count),
	  m_input_from_master("From Master", master_to_polyphonic_output_count),
	  m_automation_input("Automation", automation_input_count),
	  m_output_to_master(output_to_master_count),
	  m_sound_component_manager(channel_count)
{
	add_component(&m_gpar_input);
	add_component(&m_input_from_master);
	add_component(&m_automation_input);
	add_component(&m_output_to_master);

	m_sound_component_manager.register_observer(&m_gpar_input);

	m_output_to_master.set_buffer_ptr(output_to_master_buffer);
	m_automation_input.set_input_buffer_ptr(automation_buffer);
	m_input_from_master.set_input_buffer_ptr(master_to_polyphonic_buffer);
}

polyphonic_circuit::~polyphonic_circuit()
{

}

void polyphonic_circuit::add_sound_component(abstract_sound_component *component)
{
	/*
	 * 		check : component.channel_count == ok
	 */

	add_component(component);
	m_sound_component_manager.register_observer(component);
}

void polyphonic_circuit::set_sample_rate(const double sample_rate)
{
	m_sound_component_manager.set_current_samplerate(sample_rate);
}

bool polyphonic_circuit::process(const unsigned int channel)
{
	m_sound_component_manager.set_current_working_channel(channel);
	execute_program();
	return (m_output_to_master.last_out_was_zero());
}

void polyphonic_circuit::initialize_channel(const unsigned int channel)
{
	m_sound_component_manager.set_current_working_channel(channel);
	initialize_components();
}

void polyphonic_circuit::set_channel_gate_state(const unsigned int channel, const bool state)
{
	m_gpar_input.set_channel_gate_state(channel, state);
}

void polyphonic_circuit::set_channel_pitch(const unsigned int channel, const double pitch)
{
	m_gpar_input.set_channel_pitch(channel, pitch);
}

void polyphonic_circuit::set_channel_attack_velocity(const unsigned int channel, const double velocity)
{
	m_gpar_input.set_channel_attack_velocity(channel, velocity);
}

void polyphonic_circuit::set_channel_release_velocity(const unsigned int channel, const double velocity)
{
	m_gpar_input.set_channel_release_velocity(channel, velocity);
}

void polyphonic_circuit::notify_circuit_change()
{
	next_process_cycle();
	make_component_current_cycle_program(&m_output_to_master);
}

process::abstract_component<double> *polyphonic_circuit::get_gpar_input()
{
	return &m_gpar_input;
}

process::abstract_component<double> *polyphonic_circuit::get_master_input()
{
	return &m_input_from_master;
}

process::abstract_component<double> *polyphonic_circuit::get_automation_input()
{
	return &m_automation_input;
}

process::abstract_component<double> *polyphonic_circuit::get_output()
{
	return &m_output_to_master;
}

}
