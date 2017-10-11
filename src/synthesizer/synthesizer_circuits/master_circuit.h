#ifndef MASTER_CIRCUIT_H_
#define MASTER_CIRCUIT_H_

#include "../../process/process.h"
#include "../sound_component/sound_component.h"

namespace gammou {

class master_circuit : private process::abstract_frame<double> {




public:
	master_circuit(const unsigned int master_to_polyphonic_s_count,
				const unsigned int automation_stream_count);
	~master_circuit();

	void add_sound_component(abstract_sound_component *component);
	void process();

	void set_main_input_buffer(const double buffer[]);
	void set_master_to_polyphonic_buffer(double buffer[]);
	void set_polyphonic_to_master_buffer(double buffer[]);
	void set_main_output_buffer(double buffer[]);
	void set_automation_buffer(const double buffer[]);

	void connect_main_input_to_component(const unsigned int master_main_input_id, abstract_sound_component *component,
			const unsigned int component_input_id);
	void connect_component_to_polyphonic_output(abstract_sound_component *component, const unsigned int component_output_id,
			const unsigned int polyphonic_input_id);
	void connect_polyphonic_input_to_component(const unsigned int polyphonic_input_id, abstract_sound_component *component,
			const unsigned int component_input_id);
	void connect_component_to_main_output(abstract_sound_component *component, const unsigned int component_output_id,
			const unsigned int main_output_id);
	void connect_automation_input_to_component(const unsigned int automation_input_id, abstract_sound_component *component,
			const unsigned int component_input_id);

private:
	process::buffer_fetcher_component<double> m_main_input;
	process::buffer_filler_component<double> m_output_to_polyphonic;
	process::buffer_fetcher_component<double> m_from_polyphonic_input;
	process::buffer_filler_component<double> m_main_output;
	process::buffer_fetcher_component<double> m_automation_input;
};


}


#endif /* MASTER_CIRCUIT_H_ */
