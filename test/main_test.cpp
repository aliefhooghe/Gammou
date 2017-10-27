
#include <iostream>



#define GLOP_TEST


#ifdef GLOP_TEST
#define TEST_CHANNEL_COUNT 32




#include "../src/synthesizer/synthesizer.h"
#include "../GLOP/Glop.h"



void sound_process(void *data, float out[], const unsigned int nb_sample)
{
	gammou::synthesizer *synth  = (gammou::synthesizer*)data;

	for(unsigned int i = 0; i < nb_sample; ++i){
		double tmp;
		synth->process(nullptr, &tmp);
		out[i] = tmp * 0.1;
	}
}

#define KEY_NOTE_ON(key) \
	case XK_##key: \
		synth->send_note_on(90 + (#key)[0] - 'a', 0.8); \
		break

#define KEY_NOTE_OFF(key) \
	case XK_##key: \
		synth->send_note_off(90 + (#key)[0] - 'a', 0.8); \
		break

int key_press(void *data, const unsigned int key)
{
	gammou::synthesizer *synth  = (gammou::synthesizer*)data;

	switch (key) {

	KEY_NOTE_ON(a);
	KEY_NOTE_ON(b);
	KEY_NOTE_ON(c);
	KEY_NOTE_ON(d);
	KEY_NOTE_ON(e);
	KEY_NOTE_ON(f);
	KEY_NOTE_ON(g);
	KEY_NOTE_ON(h);
	KEY_NOTE_ON(i);
	KEY_NOTE_ON(j);
	KEY_NOTE_ON(k);
	KEY_NOTE_ON(l);
	KEY_NOTE_ON(m);
	KEY_NOTE_ON(n);
	KEY_NOTE_ON(o);
	KEY_NOTE_ON(p);
	KEY_NOTE_ON(q);
	KEY_NOTE_ON(r);
	KEY_NOTE_ON(s);
	KEY_NOTE_ON(t);
	KEY_NOTE_ON(u);
	KEY_NOTE_ON(v);
	KEY_NOTE_ON(w);
	KEY_NOTE_ON(x);
	KEY_NOTE_ON(y);
	KEY_NOTE_ON(z);
	}

	return 0;
}

int key_release(void *data, const unsigned int key)
{
	gammou::synthesizer *synth  = (gammou::synthesizer*)data;


	switch (key) {

	KEY_NOTE_OFF(a);
	KEY_NOTE_OFF(b);
	KEY_NOTE_OFF(c);
	KEY_NOTE_OFF(d);
	KEY_NOTE_OFF(e);
	KEY_NOTE_OFF(f);
	KEY_NOTE_OFF(g);
	KEY_NOTE_OFF(h);
	KEY_NOTE_OFF(i);
	KEY_NOTE_OFF(j);
	KEY_NOTE_OFF(k);
	KEY_NOTE_OFF(l);
	KEY_NOTE_OFF(m);
	KEY_NOTE_OFF(n);
	KEY_NOTE_OFF(o);
	KEY_NOTE_OFF(p);
	KEY_NOTE_OFF(q);
	KEY_NOTE_OFF(r);
	KEY_NOTE_OFF(s);
	KEY_NOTE_OFF(t);
	KEY_NOTE_OFF(u);
	KEY_NOTE_OFF(v);
	KEY_NOTE_OFF(w);
	KEY_NOTE_OFF(x);
	KEY_NOTE_OFF(y);
	KEY_NOTE_OFF(z);

	case XK_Escape:
		return 1;

	}


	return 0;
}

int main()
{
	gammou::synthesizer synth(0, 1, TEST_CHANNEL_COUNT, 0, 1, 1, 44100);
	gammou::sin_oscilator sinus(TEST_CHANNEL_COUNT);
	gammou::static_prod<2> prod(TEST_CHANNEL_COUNT);


	synth.add_sound_component_on_polyphonic_circuit(&sinus);
	synth.add_sound_component_on_polyphonic_circuit(&prod);

	synth.get_polyphonic_circuit_gpar_input()->connect_to(1, &sinus, 0);

	synth.get_polyphonic_circuit_gpar_input()->connect_to(0, &prod, 0);
	sinus.connect_to(0, &prod, 1);
	sinus.connect_to(0, &sinus, 1);

	prod.connect_to(0, synth.get_polyphonic_circuit_output(), 0);

	synth.get_master_circuit_polyphonic_input()->connect_to(0,
			synth.get_master_main_output(), 0);


	Glop_init( "Nom de la fenetre", 512, 512);
	GlopSound_init();


	Glop_set_user_data(&synth);
	Glop_set_key_press_fct(key_press);
	Glop_set_key_release_fct(key_release);

	GlopSound_start(sound_process);
	Glop_main_loop();


	GlopSound_close();
	Glop_close();

	return 0;
}



#else

#include "test_observer.h"

#include "test_component.h"



#define RUN_TEST(test_function) \
	std::cout << #test_function << ":"; \
	test_function(); \
	std::cout << " OK" << std::endl



#include <vector>

int main()
{
	RUN_TEST(test_observer);
	RUN_TEST(test_component_basic);
	RUN_TEST(test_component_with_circuit_frame);
	RUN_TEST(test_frame_component_basic);
	RUN_TEST(test_frame_component_with_circuit_frame);

	return 0;
}

#endif
