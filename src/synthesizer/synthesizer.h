#ifndef SYNTHESIZER_H_
#define SYNTHESIZER_H_


#include "sound_component/sound_component.h"
#include "synthesizer_circuits/master_circuit.h"
#include "synthesizer_circuits/polyphonic_circuit.h"


namespace gammou{


class synthesizer{



public:
	synthesizer(const unsigned int main_input_count, // todo
				const unsigned int main_output_count, // todo
				const unsigned int automation_count,
				const unsigned int master_to_polyphonic_count = 4,
				const unsigned int polyphonic_to_master_count = 2);

	~synthesizer();

	void process();

	void process_note_on(const unsigned char midi_note, const double velocity);
	void process_note_off(const unsigned char midi_note, const double velocity);


private:
	std::vector<double> m_automation_input_buffer; // Surement plutot en parametre
	std::vector<double> m_main_input_buffer; // Idem
	std::vector<double> m_main_output_buffer; // Idem

	std::vector<double> m_master_to_polyphonic_buffer;
	std::vector<double> m_polyphonic_to_master_buffer;

	polyphonic_circuit m_polyphonic_circuit;
	master_circuit m_master_circuit;

	static const double m_note_frequencies[127];
};



} /* gammou */




#endif /* SYNTHESIZER_H_ */
