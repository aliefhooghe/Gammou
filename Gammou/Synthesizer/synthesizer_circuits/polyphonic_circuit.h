#ifndef SOUND_CIRCUIT_H_
#define SOUND_CIRCUIT_H_


#include <vector>
#include "gammou_process.h"
#include "../sound_component/sound_component.h"
#include "master_circuit.h"

constexpr auto SOUND_EPSILON = 0.0003;	//( ~= 10 / (2^15));

namespace Gammou {

	namespace Sound {

		class polyphonic_circuit_midi_input : public polyphonic_sound_component {

			enum Input : unsigned int {
				GATE = 0,
				PITCH,
				ATTACK_VELOCITY,
				RELEASE_VELOCITY,
				AFTERTOUCH_PRESURE,
				COUNT};

		public:
			polyphonic_circuit_midi_input(const unsigned int channel_count);
			~polyphonic_circuit_midi_input();

			void process(const double input[]) {} // a stub
			double fetch_output(const unsigned int output_id);

			void set_channel_gate_state(const unsigned int channel, const bool gate_state);
			void set_channel_pitch(const unsigned int channel, const double pitch);
			void set_channel_attack_velocity(const unsigned int channel, const double velocity);
			void set_channel_release_velocity(const unsigned int channel, const double velocity);
			void set_aftertouch_presure(const unsigned int channel, const double presure);

		private:
			std::vector<bool> m_gate_state;
			std::vector<double> m_pitch;
			std::vector<double> m_attack_velocity;
			std::vector<double> m_release_velocity;
			std::vector<double> m_aftertouch_presure;
		};


		class polyphonic_circuit_output : public abstract_sound_component {

		public:
			polyphonic_circuit_output(std::vector<double>& output_buffer);
			~polyphonic_circuit_output() {};

			void process(const double input[]) override;
			double fetch_output(const unsigned int output_id) override {return 0.0;}  // a stub

			inline void reset_zero_flag() { m_last_out_was_zero = true;  }
			bool last_out_was_zero() const;

		private:
			double *m_buffer_ptr;
			bool m_last_out_was_zero;
		};

		/*
		 *
		 */

		class polyphonic_circuit : private Process::abstract_process_frame<double> {

			friend class synthesizer;

		public:
			polyphonic_circuit(
				master_circuit *master,
				const unsigned int channel_count,
				Process::abstract_frame_processor<double>& processor);
			~polyphonic_circuit();

			void add_sound_component(abstract_sound_component *component);
			void set_sample_rate(const double sample_rate);

			// Return whether the last output was ZERO
			bool process(const unsigned int channel);
			void initialize_channel(const unsigned int channel);

			void set_channel_gate_state(const unsigned int channel, const bool gate_state);
			void set_channel_pitch(const unsigned int channel, const double pitch);
			void set_channel_attack_velocity(const unsigned int channel, const double velocity);
			void set_channel_release_velocity(const unsigned int channel, const double velocity);
			void set_aftertouch_presure(const unsigned int channel, const double presure);

		private:
			polyphonic_circuit_midi_input m_midi_input;
			Process::vector_fetcher_component<double> m_master_output;
			Process::vector_fetcher_component<double> m_parameter_input;
			polyphonic_circuit_output m_master_input;
			sound_component_manager m_sound_component_manager;
		};

	} /* Sound */

} /* Gammou */

#endif /* SOUND_CIRCUIT_H_ */
