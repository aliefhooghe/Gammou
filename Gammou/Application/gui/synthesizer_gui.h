#pragma once

#include <mutex>
#include "../../debug.h"
#include "view.h"
#include "gui_sound_component.h"
#include "gui_synthesizer_circuit.h"


namespace Gammou {

	namespace Gui {

		class synthesizer_gui : public View::generic_window {

		public:
			synthesizer_gui(
				Sound::synthesizer *synthesizer, std::mutex *synthesizer_mutex,
				unsigned int width, const unsigned int height) 
				: View::generic_window(width, height)
			{
				set_background_color(View::cl_white);


				Sound::abstract_sound_component *osc = new Sound::sin_oscilator(12);
				Sound::abstract_sound_component *osc1 = new Sound::sin_oscilator(12);
				Sound::abstract_sound_component *osc2 = new Sound::sin_oscilator(12);


				Sound::abstract_sound_component *sum = new Sound::static_sum<5>(12);
				Sound::abstract_sound_component *sum1 = new Sound::static_sum<5>(12);
				Sound::abstract_sound_component *sum2 = new Sound::static_prod<2>(12);
				Sound::abstract_sound_component *sum3 = new Sound::static_prod<2>(12);

				synthesizer_mutex->lock();

				synthesizer->add_sound_component_on_master_circuit(osc);
				synthesizer->add_sound_component_on_master_circuit(osc1);
				synthesizer->add_sound_component_on_master_circuit(osc2);
				synthesizer->add_sound_component_on_master_circuit(sum);
				synthesizer->add_sound_component_on_master_circuit(sum1);
				synthesizer->add_sound_component_on_master_circuit(sum2);
				synthesizer->add_sound_component_on_master_circuit(sum3);

				synthesizer_mutex->unlock();

				gui_sound_component *c = new gui_sound_component(osc, synthesizer_mutex, 1, 1);
				gui_sound_component *cc = new gui_sound_component(osc2, synthesizer_mutex, 1, 1);
				gui_sound_component *ccc = new gui_sound_component(osc1, synthesizer_mutex, 1, 1);
				gui_sound_component *c2 = new gui_sound_component(sum, synthesizer_mutex, 300, 85);
				gui_sound_component *c21 = new gui_sound_component(sum1, synthesizer_mutex, 330, 100);
				gui_sound_component *c22 = new gui_sound_component(sum2, synthesizer_mutex, 360, 125);
				gui_sound_component *c23 = new gui_sound_component(sum3, synthesizer_mutex, 390, 140);

				gui_master_circuit *c_map = new gui_master_circuit(synthesizer, synthesizer_mutex, 0, 0, 1000, 600);

				c_map->add_gui_component(c);
				c_map->add_gui_component(cc);
				c_map->add_gui_component(ccc);
				c_map->add_gui_component(c2);
				c_map->add_gui_component(c21);
				c_map->add_gui_component(c22);
				c_map->add_gui_component(c23);

				add_widget(c_map);
			}
			
			~synthesizer_gui() 
			{
			}

			protected:
		};
	}
}
