
#include <limits>
#include <cmath>
#include <stdexcept>

#include "polyphonic_circuit.h"


namespace Gammou {

	namespace Sound {


		polyphonic_circuit_GPAR_input::polyphonic_circuit_GPAR_input(const unsigned int channel_count)
			: polyphonic_sound_component("Midi In", 0, Input::COUNT, channel_count),
			m_gate_state(channel_count),
			m_pitch(channel_count),
			m_attack_velocity(channel_count),
			m_release_velocity(channel_count)
		{
			// Naming
			set_output_name("Gate", 0);
			set_output_name("Pitch", 1);
			set_output_name("Attack", 2);
			set_output_name("Release", 3);
		}

		polyphonic_circuit_GPAR_input::~polyphonic_circuit_GPAR_input()
		{
		}

		double polyphonic_circuit_GPAR_input::fetch_output(const unsigned int output_id)
		{
			const unsigned int channel = get_current_working_channel();

			switch( output_id ) {

			case Input::GATE:
				return static_cast<double>(m_gate_state[channel]);
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

			return 0.0; // pour le compilo
		}

		void polyphonic_circuit_GPAR_input::set_channel_gate_state(const unsigned int channel, const bool state)
		{
			m_gate_state[channel] = state;
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

		polyphonic_circuit_output::polyphonic_circuit_output(std::vector<double>& output_buffer)
			: 
			abstract_sound_component("Master In", static_cast<unsigned int>(output_buffer.size()), 0),
			m_buffer_ptr(output_buffer.data()),
			m_last_out_was_zero(false)
		{
		}

		void polyphonic_circuit_output::process(const double input[])
		{

			if( m_buffer_ptr != nullptr ){
				const unsigned int ic = get_input_count();

				for(unsigned int i = 0; i < ic; ++i){
					m_buffer_ptr[i] += input[i];

					if(std::abs(input[i]) > SOUND_EPSILON)
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

		polyphonic_circuit::polyphonic_circuit(master_circuit * master, const unsigned int channel_count)
			:
			m_gpar_input(channel_count),
			m_input_from_master("Master Out", master->m_master_to_polyphonic_buffer),
			m_parameter_input("Parameters", master->m_parameter_buffer),
			m_output_to_master(master->m_polyphonic_to_master_buffer),
			m_sound_component_manager(channel_count)
		{
			add_sound_component(&m_gpar_input);
			add_component(&m_input_from_master);
			add_component(&m_parameter_input);
			add_component(&m_output_to_master);
		}

		polyphonic_circuit::~polyphonic_circuit()
		{
		}

		void polyphonic_circuit::add_sound_component(polyphonic_sound_component *component)
		{
			// Component channel alredy checked and Initialized by synthesizer
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
			m_output_to_master.reset_zero_flag();
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

} /* Sound */

}	/* Gammou */
