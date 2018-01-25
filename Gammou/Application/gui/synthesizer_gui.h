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
				Sound::abstract_sound_component *sum = new Sound::static_sum<10>(12);
				Sound::abstract_sound_component *sum1 = new Sound::static_sum<5>(12);
				Sound::abstract_sound_component *sum2 = new Sound::static_sum<3>(12);
				Sound::abstract_sound_component *sum3 = new Sound::static_sum<2>(12);

				synthesizer_mutex->lock();

				synthesizer->add_sound_component_on_master_circuit(osc);
				synthesizer->add_sound_component_on_master_circuit(sum);
				synthesizer->add_sound_component_on_master_circuit(sum1);
				synthesizer->add_sound_component_on_master_circuit(sum2);
				synthesizer->add_sound_component_on_master_circuit(sum3);

				synthesizer_mutex->unlock();

				gui_sound_component *c = new gui_sound_component(osc, 1, 1);
				gui_sound_component *c2 = new gui_sound_component(sum, 300, 85);
				gui_sound_component *c21 = new gui_sound_component(sum1, 330, 100);
				gui_sound_component *c22 = new gui_sound_component(sum2, 360, 125);
				gui_sound_component *c23 = new gui_sound_component(sum3, 390, 140);

				gui_master_circuit *c_map = new gui_master_circuit(synthesizer, synthesizer_mutex, 0, 0, 1000, 600);

				c_map->add_gui_component(c);
				c_map->add_gui_component(c2);
				c_map->add_gui_component(c21);
				c_map->add_gui_component(c22);
				c_map->add_gui_component(c23);

				add_widget(c_map);
			}
			
			~synthesizer_gui() 
			{
			}

			bool on_mouse_dbl_click(const int x, const int y) override
			{
				DEBUG_PRINT("DBLCLICK\n");
				return true;
			}

			protected:
		};
	}
}
