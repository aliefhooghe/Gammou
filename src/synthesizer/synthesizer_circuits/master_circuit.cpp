/*
 * master_circuit.cpp
 *
 *  Created on: 11 oct. 2017
 *      Author: liefhooghe
 */

#include "master_circuit.h"

namespace gammou {



master_circuit::master_circuit(const unsigned int master_to_polyphonic_output_count,
		const unsigned int automation_input_count)
	: m_main_input(1), // Tempo
	  m_output_to_polyphonic("To Polyphonic", master_to_polyphonic_output_count),
	  m_from_polyphonic_input("From Polyphonic", 2), // left, right
	  m_main_output(2),
	  m_automation_input("Automation", automation_input_count),
	  m_sound_component_manager(1)
{
	add_component(&m_main_input);
	add_component(&m_output_to_polyphonic);
	add_component(&m_from_polyphonic_input);
	add_component(&m_main_output);
	add_component(&m_automation_input);
}

master_circuit::~master_circuit()
{

}


void master_circuit::add_sound_component(abstract_sound_component *component)
{
	/*
	 * 	check : component.is_multichannel = false
	 */
	add_component(component);
	m_sound_component_manager.register_observer(component);
}

void master_circuit::set_sample_rate(const double sample_rate)
{
	m_sound_component_manager.set_current_samplerate(sample_rate);
}

void master_circuit::process()
{
	execute_program();
}

void master_circuit::set_main_input_buffer(const double buffer[])
{
	m_main_input.set_input_buffer_ptr(buffer);
}

void master_circuit::set_master_to_polyphonic_buffer(double buffer[])
{
	m_output_to_polyphonic.set_output_pointer(buffer);
}

void master_circuit::set_polyphonic_to_master_buffer(double buffer[])
{
	m_from_polyphonic_input.set_input_buffer_ptr(buffer);
}

void master_circuit::set_main_output_buffer(double buffer[])
{
	m_main_output.set_output_pointer(buffer);
}

void master_circuit::set_automation_buffer(const double buffer[])
{
	m_automation_input.set_input_buffer_ptr(buffer);
}

void master_circuit::connect_main_input_to_component(const unsigned int master_main_input_id, abstract_sound_component *component,
		const unsigned int component_input_id)
{
	m_main_input.connect_to(master_main_input_id, component, component_input_id);
}

void master_circuit::connect_component_to_polyphonic_output(abstract_sound_component *component, const unsigned int component_output_id,
		const unsigned int polyphonic_input_id)
{
	component->connect_to(component_output_id, &m_output_to_polyphonic, polyphonic_input_id);
}

void master_circuit::connect_polyphonic_input_to_component(const unsigned int polyphonic_input_id, abstract_sound_component *component,
		const unsigned int component_input_id)
{
	m_from_polyphonic_input.connect_to(polyphonic_input_id, component, component_input_id);
}

void master_circuit::connect_component_to_main_output(abstract_sound_component *component, const unsigned int component_output_id,
		const unsigned int main_output_id)
{
	component->connect_to(component_output_id, &m_main_output, main_output_id);
}

void master_circuit::connect_automation_input_to_component(const unsigned int automation_input_id, abstract_sound_component *component,
		const unsigned int component_input_id)
{
	m_automation_input.connect_to(automation_input_id, component, component_input_id);
}



}  // namespace gammou
