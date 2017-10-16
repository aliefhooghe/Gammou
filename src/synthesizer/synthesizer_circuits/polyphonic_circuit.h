#ifndef SOUND_CIRCUIT_H_
#define SOUND_CIRCUIT_H_


#include <vector>
#include "../../process/process.h"
#include "../sound_component/sound_component.h"


namespace gammou {

// 'GPAR' = Gate, Pitch, Attack, Release

class polyphonic_circuit_GPAR_input : public abstract_sound_component {

	enum Input {GATE = 0, PITCH = 1, ATTACK_VELOCITY = 2, RELEASE_VELOCITY = 3, COUNT = 4};

public:
	polyphonic_circuit_GPAR_input(const unsigned int channel_count);
	~polyphonic_circuit_GPAR_input();

	void process(const double input[]) {} // a stub
	double fetch_output(const unsigned int output_id);

	void set_channel_gate_state(const unsigned int channel, const bool gate_state);
	void set_channel_pitch(const unsigned int channel, const double pitch);
	void set_channel_attack_velocity(const unsigned int channel, const double velocity);
	void set_channel_release_velocity(const unsigned int channel, const double velocity);

private:
	std::vector<bool> m_gate_state;
	std::vector<double> m_pitch;
	std::vector<double> m_attack_velocity;
	std::vector<double> m_release_velocity;
};




/*
 * 	input : pitch, attack, release, c0, ...., cn
 */

class polyphonic_circuit : private process::abstract_frame<double> {

public:
	polyphonic_circuit(const unsigned int channel_count,
			const unsigned int master_to_polyphonic_output_count,
			const unsigned int automation_input_count);
	~polyphonic_circuit();

	void add_sound_component(abstract_sound_component *component);
	void set_sample_rate(const double sample_rate);

	void process(const unsigned int channel);
	void initialize_channel(const unsigned int channel);

	void set_channel_gate_state(const unsigned int channel, const bool gate_state);
	void set_channel_pitch(const unsigned int channel, const double pitch);
	void set_channel_attack_velocity(const unsigned int channel, const double velocity);
	void set_channel_release_velocity(const unsigned int channel, const double velocity);

	void set_output_to_master_buffer(double buffer[]);
	void set_automation_buffer(const double buffer[]);
	void set_master_to_polyphonic_buffer(const double buffer[]);

	void connect_gpar_input_to_component(const unsigned int gpar_output_id, abstract_sound_component *component,
			const unsigned int component_input_id);
	void connect_master_input_to_component(const unsigned int master_input_id, abstract_sound_component *component,
			const unsigned component_input_id);
	void connect_automtion_input_to_component(const unsigned int automation_input_id, abstract_sound_component *component,
			const unsigned int component_input_id);
	void connect_component_to_output(abstract_sound_component *component, const unsigned int component_output_id,
			const unsigned int circuit_output_id);
private:
	void notify_circuit_change();
	polyphonic_circuit_GPAR_input m_gpar_input;
	process::buffer_fetcher_component<double> m_input_from_master;
	process::buffer_fetcher_component<double> m_automation_input;
	process::buffer_sum_component<double> m_output_to_master;
	sound_component_manager m_sound_component_manager;
};


}
#endif /* SOUND_CIRCUIT_H_ */
