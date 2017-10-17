#ifndef SYNTHESIZER_H_
#define SYNTHESIZER_H_


#include "sound_component/sound_component.h"
#include "synthesizer_circuits/master_circuit.h"
#include "synthesizer_circuits/polyphonic_circuit.h"


namespace gammou{






class synthesizer{

public:

	void process_note_on(const unsigned char note, const double velocity);
	void process_note_off(const unsigned char note, const double velocity);


private:
	double *m_automation_input_buffer;
	double *m_main_input_buffer;
	double *m_main_output_buffer;

	double *m_master_to_polyphonic_buffer;
	double m_polyphonic_to_master_buffer[2];

	polyphonic_circuit m_polyphonic_circuit;
	master_circuit m_master_circuit;


	static const double m_note_frequencies[127];
};


} /* gammou */




#endif /* SYNTHESIZER_H_ */
