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
				const unsigned int input_from_polyphonic_count,
				double master_to_polyphonic_buffer[],
				const double polyphonic_to_master_buffer[],
				const double automation_buffer[]
				)
			: m_main_input(main_input_count),
			m_output_to_polyphonic("To Polyphonic", master_to_polyphonic_output_count),
			m_from_polyphonic_input("From Polyphonic", input_from_polyphonic_count),
			m_main_output(main_output_count),
			m_automation_input("Automation", automation_input_count),
			m_sound_component_manager(1)
		{
			add_component(&m_main_input);
			add_component(&m_output_to_polyphonic);
			add_component(&m_from_polyphonic_input);
			add_component(&m_main_output);
			add_component(&m_automation_input);

			m_output_to_polyphonic.set_output_pointer(master_to_polyphonic_buffer);
			m_from_polyphonic_input.set_input_buffer_ptr(polyphonic_to_master_buffer);
			m_automation_input.set_input_buffer_ptr(automation_buffer);
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

		Process::abstract_component<double> *master_circuit::get_main_input()
		{
			return &m_main_input;
		}

		Process::abstract_component<double> *master_circuit::get_main_output()
		{
			return &m_main_output;
		}

		Process::abstract_component<double> *master_circuit::get_polyphonic_input()
		{
			return &m_from_polyphonic_input;
		}

		Process::abstract_component<double> *master_circuit::get_polyphonic_output()
		{
			return &m_output_to_polyphonic;
		}

		Process::abstract_component<double> *master_circuit::get_automation_input()
		{
			return &m_automation_input;
		}

		void master_circuit::notify_circuit_change()
		{
			next_process_cycle();
			make_component_current_cycle_program(&m_main_output);
		}

	} /* Sound */

}  // namespace Gammou
