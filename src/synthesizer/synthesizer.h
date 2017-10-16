#ifndef SYNTHESIZER_H_
#define SYNTHESIZER_H_


#include "sound_component/sound_component.h"

#include


namespace gammou{






class synthesizer{

public:



	void process_note_on(const unsigned char note, const double velocity);
	void process_note_off(const unsigned char note, const double velocity);

private:

};


} /* gammou */




#endif /* SYNTHESIZER_H_ */
