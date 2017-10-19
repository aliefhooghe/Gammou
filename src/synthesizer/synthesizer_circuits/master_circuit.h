#ifndef MASTER_CIRCUIT_H_
#define MASTER_CIRCUIT_H_

#include "../../process/process.h"
#include "../sound_component/sound_component.h"

namespace gammou {

class master_circuit : private process::abstract_frame<double> {

public:
	master_circuit(const unsigned int master_to_polyphonic_output_count,
				const unsigned int automation_input_count,
				const unsigned int main_input_count,
				const unsigned int main_output_count,
				const unsigned int input_from_polyphonic_count,
				const double main_input_buffer[],
				double master_to_polyphonic_buffer[],
				const double polyphonic_to_master_buffer[],
				const double automation_buffer[],
				double main_output_buffer[]
				);
	~master_circuit();

	void add_sound_component(abstract_sound_component *component);
	void set_sample_rate(const double sample_rate);
	void process();

	process::abstract_component<double> *get_main_input();
	process::abstract_component<double> *get_main_output();

	process::abstract_component<double> *get_polyphonic_input();
	process::abstract_component<double> *get_polyphonic_output();
	process::abstract_component<double> *get_automation_input();

private:

	void notify_circuit_change() override;
	process::buffer_fetcher_component<double> m_main_input;
	process::buffer_filler_component<double> m_output_to_polyphonic;
	process::buffer_fetcher_component<double> m_from_polyphonic_input;
	process::buffer_filler_component<double> m_main_output;
	process::buffer_fetcher_component<double> m_automation_input;
	sound_component_manager m_sound_component_manager;
};


}


#endif /* MASTER_CIRCUIT_H_ */
