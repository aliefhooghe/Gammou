/*
 * master_circuit.cpp
 *
 *  Created on: 11 oct. 2017
 *      Author: liefhooghe

 */


#include "master_circuit.h"

#define MASTER_VOLUME_SMOOTHING_TAU 0.1

namespace Gammou {

	namespace Sound {
			

		master_circuit::master_circuit(
			const unsigned int master_to_polyphonic_output_count, 
			const unsigned int parameter_input_count, 
			const unsigned int main_input_count, 
			const unsigned int main_output_count, 
			const unsigned int input_from_polyphonic_count)
			:
			m_volume_order(1.0),
			m_volume(1.0),
			m_volume_smoothing_fact(0.0),
			m_master_to_polyphonic_buffer(master_to_polyphonic_output_count),
			m_polyphonic_to_master_buffer(input_from_polyphonic_count),
			m_parameter_buffer(parameter_input_count), 
			m_main_input("Input", main_input_count),
			m_polyphonic_input("Polyphonic In", m_master_to_polyphonic_buffer),
			m_polyphonic_output("Polyphonic Out", m_polyphonic_to_master_buffer),
			m_main_output("Output", main_output_count),
			m_parameter_input("Parameters", m_parameter_buffer),
			m_sound_component_manager(1) //  1 channel
		{
			add_component(&m_main_input);
			add_component(&m_polyphonic_input);
			add_component(&m_polyphonic_output);
			add_component(&m_main_output);
			add_component(&m_parameter_input);

			// Naming

			for (unsigned int i = 0; i < parameter_input_count; ++i)
				m_parameter_input.set_output_name(Process::abstract_component<double>::default_input_name(i), i);
			for(unsigned int i = 0; i < main_input_count; ++i)
				m_main_input.set_output_name(Process::abstract_component<double>::default_input_name(i), i);
			for (unsigned int i = 0; i < main_output_count; ++i)
				m_main_output.set_input_name(Process::abstract_component<double>::default_output_name(i), i);
		}

		master_circuit::~master_circuit()
		{
			DEBUG_PRINT("Master DTOR\n");
		}

		void master_circuit::add_sound_component(abstract_sound_component *component)
		{
			add_component(component);
			m_sound_component_manager.register_observer(component);
			component->set_sample_rate(m_sound_component_manager.get_current_sample_rate());
			component->set_current_working_channel(0);
			component->initialize_process();
		}

		void master_circuit::set_sample_rate(const double sample_rate)
		{
			//DEBUG_PRINT("SET MMASTER SAMPLERATE\n");
			m_sound_component_manager.set_current_samplerate(sample_rate);
			m_volume_smoothing_fact = MASTER_VOLUME_SMOOTHING_TAU * sample_rate;
		}

		void master_circuit::process(const double input[], double output[])
		{
			m_main_input.set_input_buffer_ptr(input);
			m_main_output.set_output_pointer(output);
			execute_program();

			// Apply master volume
			const unsigned int oc = m_main_output.get_input_count(); // master's output are main_output's input

			for (unsigned int i = 0; i < oc; ++i)
				output[i] *= m_volume;
			
			m_volume = (m_volume_order + m_volume_smoothing_fact * m_volume) / (1.0 + m_volume_smoothing_fact);
		}

		void master_circuit::set_volume(const double volume_order)
		{
			//DEBUG_PRINT("MASTER Volume = %lf\n", volume_order);
			m_volume_order = volume_order;
		}

		void master_circuit::notify_circuit_change()
		{
			DEBUG_PRINT("Compiling Master Circuit\n");
			next_process_cycle();
			make_component_current_cycle_program(&m_main_output);
			make_component_current_cycle_program(&m_polyphonic_input);
		}

	} /* Sound */

}  // namespace Gammou
