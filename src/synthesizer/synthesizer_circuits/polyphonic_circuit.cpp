
#include "polyphonic_circuit.h"


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


polyphonic_circuit::polyphonic_circuit(const unsigned int channel_count,
		const unsigned int master_to_polyphonic_output_count,
		const unsigned int automation_input_count)
	: m_gpar_input(channel_count),
	  m_input_from_master("From Master", master_to_polyphonic_output_count),
	  m_automation_input("Automation", automation_input_count),
	  m_output_to_master(2), // L, R
	  m_sound_component_manager(channel_count)
{
	add_component(&m_gpar_input);
	add_component(&m_input_from_master);
	add_component(&m_automation_input);
	add_component(&m_output_to_master);

	m_sound_component_manager.register_observer(&m_gpar_input);
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

void polyphonic_circuit::process(const unsigned int channel)
{
	m_sound_component_manager.set_current_working_channel(channel);
	execute_program();
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

void polyphonic_circuit::set_output_to_master_buffer(double buffer[])
{
	m_output_to_master.set_buffer_ptr(buffer);
}


void polyphonic_circuit::set_automation_buffer(const double buffer[])
{
	m_automation_input.set_input_buffer_ptr(buffer);
}

void polyphonic_circuit::set_master_to_polyphonic_buffer(const double buffer[])
{
	m_input_from_master.set_input_buffer_ptr(buffer);
}

void polyphonic_circuit::notify_circuit_change()
{
	next_process_cycle();
	make_component_current_cycle_program(&m_output_to_master);
}

void polyphonic_circuit::connect_gpar_input_to_component(const unsigned int gpar_output_id, abstract_sound_component *component,
		const unsigned int component_input_id)
{
	m_gpar_input.connect_to(gpar_output_id, component, component_input_id);
}

void polyphonic_circuit::connect_master_input_to_component(const unsigned int master_input_id, abstract_sound_component *component,
		const unsigned component_input_id)
{
	m_input_from_master.connect_to(master_input_id, component, component_input_id);
}

void polyphonic_circuit::connect_automtion_input_to_component(const unsigned int automation_input_id, abstract_sound_component *component,
		const unsigned int component_input_id)
{
	m_automation_input.connect_to(automation_input_id, component, component_input_id);
}

void polyphonic_circuit::connect_component_to_output(abstract_sound_component *component, const unsigned int component_output_id,
		const unsigned int circuit_output_id)
{
	component->connect_to(component_output_id, &m_output_to_master, circuit_output_id);
}

}
