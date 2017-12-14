#ifndef MASTER_CIRCUIT_H_
#define MASTER_CIRCUIT_H_

#include "process.h"
#include "../sound_component/sound_component.h"



namespace Gammou {

	namespace Sound {

		class master_circuit : private Process::abstract_frame<double> {

		public:
			master_circuit(const unsigned int master_to_polyphonic_output_count,
						const unsigned int automation_input_count,
						const unsigned int main_input_count,
						const unsigned int main_output_count,
						const unsigned int input_from_polyphonic_count,
						double master_to_polyphonic_buffer[],
						const double polyphonic_to_master_buffer[],
						const double automation_buffer[]
						);
			~master_circuit();

			void add_sound_component(abstract_sound_component *component);
			void set_sample_rate(const double sample_rate);
			void process(const double input[], double output[]);

			Process::abstract_component<double> *get_main_input();
			Process::abstract_component<double> *get_main_output();

			Process::abstract_component<double> *get_polyphonic_input();
			Process::abstract_component<double> *get_polyphonic_output();
			Process::abstract_component<double> *get_automation_input();

		private:
			void notify_circuit_change() override;
			
			Process::buffer_fetcher_component<double> m_main_input;
			Process::buffer_filler_component<double> m_output_to_polyphonic;
			Process::buffer_fetcher_component<double> m_from_polyphonic_input;
			Process::buffer_filler_component<double> m_main_output;
			Process::buffer_fetcher_component<double> m_automation_input;
			sound_component_manager m_sound_component_manager;
		};

	} /* Sound */


}	/* Gammou */


#endif /* MASTER_CIRCUIT_H_ */
