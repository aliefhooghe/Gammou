/*
 * master_circuit.cpp
 *
 *  Created on: 11 oct. 2017
 *      Author: liefhooghe

 */

#include "master_circuit.h"

namespace Gammou {

	namespace Sound {
			

		master_circuit::master_circuit(
			const unsigned int master_to_polyphonic_output_count, 
			const unsigned int automation_input_count, 
			const unsigned int main_input_count, 
			const unsigned int main_output_count, 
			const unsigned int input_from_polyphonic_count)
			:
			m_master_to_polyphonic_buffer(master_to_polyphonic_output_count),
			m_polyphonic_to_master_buffer(input_from_polyphonic_count),
			m_automation_buffer(automation_input_count), 
			m_main_input("Input", main_input_count),
			m_output_to_polyphonic("Polyphonic In", m_master_to_polyphonic_buffer),
			m_from_polyphonic_input("Polyphonic Out", m_polyphonic_to_master_buffer),
			m_main_output("Output", main_output_count),
			m_automation_input("Automations", m_automation_buffer),
			m_sound_component_manager(1) //  1 channel
		{
			add_component(&m_main_input);
			add_component(&m_output_to_polyphonic);
			add_component(&m_from_polyphonic_input);
			add_component(&m_main_output);
			add_component(&m_automation_input);

			// Naming

			for (unsigned int i = 0; i < automation_input_count; ++i)
				m_automation_input.set_output_name(Process::abstract_component<double>::default_input_name(i), i);
			for(unsigned int i = 0; i < main_input_count; ++i)
				m_main_input.set_output_name(Process::abstract_component<double>::default_input_name(i), i);
			for (unsigned int i = 0; i < main_output_count; ++i)
				m_main_output.set_input_name(Process::abstract_component<double>::default_output_name(i), i);
		}

		master_circuit::~master_circuit()
		{

		}

		void master_circuit::add_sound_component(abstract_sound_component *component)
		{
			add_component(component);
			m_sound_component_manager.register_observer(component);
		}

		void master_circuit::set_sample_rate(const double sample_rate)
		{
			m_sound_component_manager.set_current_samplerate(sample_rate);
		}

		void master_circuit::process(const double input[], double output[])
		{
			m_main_input.set_input_buffer_ptr(input);
			m_main_output.set_output_pointer(output);
			execute_program();

		}

		void master_circuit::notify_circuit_change()
		{
			next_process_cycle();
			make_component_current_cycle_program(&m_main_output);
		}

	} /* Sound */

}  // namespace Gammou
