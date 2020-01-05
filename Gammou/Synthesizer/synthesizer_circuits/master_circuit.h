#ifndef MASTER_CIRCUIT_H_
#define MASTER_CIRCUIT_H_

#include "../../debug.h"
#include "gammou_process.h"
#include "../sound_component/sound_component.h"



namespace Gammou {

	namespace Sound {

		/*
		
		*/

		class master_circuit : private Process::abstract_process_frame<double> {

		friend class polyphonic_circuit;
		friend class synthesizer;

		public:
			master_circuit(
						const unsigned int master_to_polyphonic_output_count,
						const unsigned int parameter_input_count,
						const unsigned int main_input_count,
						const unsigned int main_output_count,
						const unsigned int input_from_polyphonic_count,
						Process::abstract_frame_processor<double>& processor
						);

			~master_circuit();

			void add_sound_component(abstract_sound_component *component);
			void set_sample_rate(const double sample_rate);
			void process(const double input[], double output[]);
			void set_volume(const double volume_order);

			void set_pitch_bend(const double value) { m_parameter_buffer[0] = value; }

		private:
			double m_volume_order;
			double m_volume;
			double m_volume_smoothing_fact;

			std::vector<double> m_master_to_polyphonic_buffer;
			std::vector<double> m_polyphonic_to_master_buffer;
			std::vector<double> m_parameter_buffer;

			Process::vector_fetcher_component<double> m_main_input;
			Process::vector_filler_component<double> m_polyphonic_input;
			Process::vector_fetcher_component<double> m_polyphonic_output;
			Process::vector_filler_component<double> m_main_output;
			Process::vector_fetcher_component<double> m_parameter_input;

			sound_component_manager m_sound_component_manager;
		};

	} /* Sound */


}	/* Gammou */


#endif /* MASTER_CIRCUIT_H_ */
