#pragma once

#include <mutex>
#include "../../debug.h"
#include "view.h"
#include "gui_sound_component.h"
#include "gui_synthesizer_circuit.h"



#define GAMMOU_SYNTHESIZER_CHANNEL_COUNT 128


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

				/*
						MASTER
				*/

				Sound::sound_component *osc = new Sound::sin_oscilator(1);
				Sound::sound_component *osc1 = new Sound::sin_oscilator(1);
				Sound::sound_component *osc2 = new Sound::sin_oscilator(1);


				Sound::sound_component *sum = new Sound::static_sum<5>(1);
				Sound::sound_component *sum1 = new Sound::static_sum<5>(1);
				Sound::sound_component *sum2 = new Sound::static_prod<2>(1);
				Sound::sound_component *sum3 = new Sound::static_prod<2>(1);

				Sound::sound_component *f1 = new Sound::fpb1(1);
				Sound::sound_component *f2 = new Sound::fpb1(1);

				synthesizer_mutex->lock();

				synthesizer->add_sound_component_on_master_circuit(osc);
				synthesizer->add_sound_component_on_master_circuit(osc1);
				synthesizer->add_sound_component_on_master_circuit(osc2);
				synthesizer->add_sound_component_on_master_circuit(sum);
				synthesizer->add_sound_component_on_master_circuit(sum1);
				synthesizer->add_sound_component_on_master_circuit(sum2);
				synthesizer->add_sound_component_on_master_circuit(sum3);
				synthesizer->add_sound_component_on_master_circuit(f1);
				synthesizer->add_sound_component_on_master_circuit(f2);

				synthesizer_mutex->unlock();

				gui_sound_component *c = new gui_sound_component(osc, synthesizer_mutex, 1, 1);
				gui_sound_component *cc = new gui_sound_component(osc2, synthesizer_mutex, 1, 1);
				gui_sound_component *ccc = new gui_sound_component(osc1, synthesizer_mutex, 1, 1);
				gui_sound_component *c2 = new gui_sound_component(sum, synthesizer_mutex, 300, 85);
				gui_sound_component *c21 = new gui_sound_component(sum1, synthesizer_mutex, 330, 100);
				gui_sound_component *c22 = new gui_sound_component(sum2, synthesizer_mutex, 360, 125);
				gui_sound_component *c23 = new gui_sound_component(sum3, synthesizer_mutex, 390, 140);
				gui_sound_component *gf1 = new gui_sound_component(f1, synthesizer_mutex, 410, 160);
				gui_sound_component *gf2 = new gui_sound_component(f2, synthesizer_mutex, 430, 180);

				gui_master_circuit *master = new gui_master_circuit(synthesizer, synthesizer_mutex, 0, 0, 800, 800, View::create_color((unsigned char)50u, (unsigned char)50u, (unsigned char)50u));

				master->add_gui_component(c);
				master->add_gui_component(cc);
				master->add_gui_component(ccc);
				master->add_gui_component(c2);
				master->add_gui_component(c21);
				master->add_gui_component(c22);
				master->add_gui_component(c23);
				master->add_gui_component(gf1);
				master->add_gui_component(gf2);

				/*
					POLYPHONIC
				*/

				Sound::sound_component *posc = new Sound::sin_oscilator(GAMMOU_SYNTHESIZER_CHANNEL_COUNT);
				Sound::sound_component *posc1 = new Sound::sin_oscilator(GAMMOU_SYNTHESIZER_CHANNEL_COUNT);
				Sound::sound_component *posc2 = new Sound::sin_oscilator(GAMMOU_SYNTHESIZER_CHANNEL_COUNT);


				Sound::sound_component *psum = new Sound::static_sum<5>(GAMMOU_SYNTHESIZER_CHANNEL_COUNT);
				Sound::sound_component *psum1 = new Sound::static_sum<5>(GAMMOU_SYNTHESIZER_CHANNEL_COUNT);
				Sound::sound_component *psum2 = new Sound::static_prod<2>(GAMMOU_SYNTHESIZER_CHANNEL_COUNT);
				Sound::sound_component *psum3 = new Sound::static_prod<2>(GAMMOU_SYNTHESIZER_CHANNEL_COUNT);


				Sound::sound_component *pf1 = new Sound::fpb1(GAMMOU_SYNTHESIZER_CHANNEL_COUNT);
				Sound::sound_component *pf2 = new Sound::fpb1(GAMMOU_SYNTHESIZER_CHANNEL_COUNT);

				Sound::sound_component *ppsum = new Sound::static_sum<5>(GAMMOU_SYNTHESIZER_CHANNEL_COUNT);
				Sound::sound_component *ppsum1 = new Sound::static_sum<5>(GAMMOU_SYNTHESIZER_CHANNEL_COUNT);

				synthesizer_mutex->lock();

				synthesizer->add_sound_component_on_polyphonic_circuit(posc);
				synthesizer->add_sound_component_on_polyphonic_circuit(posc1);
				synthesizer->add_sound_component_on_polyphonic_circuit(psum3);
				synthesizer->add_sound_component_on_polyphonic_circuit(posc2);
				synthesizer->add_sound_component_on_polyphonic_circuit(psum);
				synthesizer->add_sound_component_on_polyphonic_circuit(psum1);
				synthesizer->add_sound_component_on_polyphonic_circuit(psum2);
				synthesizer->add_sound_component_on_polyphonic_circuit(pf1);
				synthesizer->add_sound_component_on_polyphonic_circuit(pf2);
				synthesizer->add_sound_component_on_polyphonic_circuit(ppsum);
				synthesizer->add_sound_component_on_polyphonic_circuit(ppsum1);


				synthesizer_mutex->unlock();

				gui_sound_component *pc = new gui_sound_component(posc, synthesizer_mutex, 1, 1);
				gui_sound_component *pcc = new gui_sound_component(posc2, synthesizer_mutex, 1, 1);
				gui_sound_component *pccc = new gui_sound_component(posc1, synthesizer_mutex, 1, 1);
				gui_sound_component *pc2 = new gui_sound_component(psum, synthesizer_mutex, 300, 85);
				gui_sound_component *pc21 = new gui_sound_component(psum1, synthesizer_mutex, 330, 100);
				gui_sound_component *pc22 = new gui_sound_component(psum2, synthesizer_mutex, 360, 125);
				gui_sound_component *pc23 = new gui_sound_component(psum3, synthesizer_mutex, 390, 140);
				gui_sound_component *pgf1 = new gui_sound_component(pf1, synthesizer_mutex, 410, 160);
				gui_sound_component *pgf2 = new gui_sound_component(pf2, synthesizer_mutex, 430, 180);

				gui_sound_component *ppc2 = new gui_sound_component(ppsum, synthesizer_mutex, 300, 85);
				gui_sound_component *ppc21 = new gui_sound_component(ppsum1, synthesizer_mutex, 330, 100);

				gui_polyphonic_circuit *poly = new gui_polyphonic_circuit(synthesizer, synthesizer_mutex, 0, 0, 800, 800, View::create_color((unsigned char)50u, (unsigned char)50u, (unsigned char)50u));

				poly->add_gui_component(pc);
				poly->add_gui_component(pcc);
				poly->add_gui_component(pccc);
				poly->add_gui_component(pc2);
				poly->add_gui_component(pc21);
				poly->add_gui_component(pc22);
				poly->add_gui_component(pc23);
				poly->add_gui_component(pgf1);
				poly->add_gui_component(pgf2);
				poly->add_gui_component(ppc2);
				poly->add_gui_component(ppc21);


				/////////

				pages = new View::page_container(0, 0, 800, 800, View::cl_bisque);

				pages->add_page(master);
				pages->add_page(poly);
				page_id = 0;
				pages->select_page(page_id);
	
				add_widget(pages);
	
				add_widget(new View::push_button([&] 
				{ 
					page_id = (page_id == 0) ? 1 : 0;
					pages->select_page(page_id);
				}
				, "Change page", 730, 0));
			}
			
			~synthesizer_gui() 
			{
			}

		private:
			View::page_container *pages;
			unsigned int page_id;
		};
	}
}
