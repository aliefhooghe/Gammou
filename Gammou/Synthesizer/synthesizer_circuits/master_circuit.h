#ifndef MASTER_CIRCUIT_H_
#define MASTER_CIRCUIT_H_

#include "gammou_process.h"
#include "../sound_component/sound_component.h"



namespace Gammou {

	namespace Sound {

		/*
		
		*/

		class master_circuit : private Process::abstract_frame<double> {

		friend class polyphonic_circuit;
		friend class synthesizer;

		public:
			master_circuit(
						const unsigned int master_to_polyphonic_output_count,
						const unsigned int parameter_input_count,
						const unsigned int main_input_count,
						const unsigned int main_output_count,
						const unsigned int input_from_polyphonic_count
						);

			~master_circuit();

			void add_sound_component(abstract_sound_component *component);
			void set_sample_rate(const double sample_rate);
			void process(const double input[], double output[]);

			//void 
		private:
			void notify_circuit_change() override;
			
			std::vector<double> m_master_to_polyphonic_buffer;
			std::vector<double> m_polyphonic_to_master_buffer;
			std::vector<double> m_parameter_buffer;

			Process::vector_fetcher_component<double> m_main_input;
			Process::vector_filler_component<double> m_output_to_polyphonic;
			Process::vector_fetcher_component<double> m_from_polyphonic_input;
			Process::vector_filler_component<double> m_main_output;
			Process::vector_fetcher_component<double> m_parameter_input;

			sound_component_manager m_sound_component_manager;
		};

	} /* Sound */


}	/* Gammou */


#endif /* MASTER_CIRCUIT_H_ */
