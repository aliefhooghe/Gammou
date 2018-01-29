#include "synthesizer_gui.h"


namespace Gammou {

	namespace Gui {

		synthesizer_gui::synthesizer_gui(Sound::synthesizer * synthesizer, std::mutex * synthesizer_mutex, unsigned int width, 
			const unsigned int height)
			: View::generic_window(width, height)
		{

			set_background_color(View::cl_chartreuse); // for gui debuging

			/*
			MASTER
			*/


			//

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

			gui_master_circuit *gui_master = new gui_master_circuit(&m_main_factory, synthesizer, synthesizer_mutex, 0, 0, 800, 800);

			gui_master->add_gui_component(c);
			gui_master->add_gui_component(cc);
			gui_master->add_gui_component(ccc);
			gui_master->add_gui_component(c2);
			gui_master->add_gui_component(c21);
			gui_master->add_gui_component(c22);
			gui_master->add_gui_component(c23);
			gui_master->add_gui_component(gf1);
			gui_master->add_gui_component(gf2);

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

			Sound::sound_component *pm1 = new Sound::static_prod<2>(GAMMOU_SYNTHESIZER_CHANNEL_COUNT);
			Sound::sound_component *pm2 = new Sound::static_prod<2>(GAMMOU_SYNTHESIZER_CHANNEL_COUNT);
			Sound::sound_component *pm3 = new Sound::static_prod<2>(GAMMOU_SYNTHESIZER_CHANNEL_COUNT);
			Sound::sound_component *pm4 = new Sound::static_prod<2>(GAMMOU_SYNTHESIZER_CHANNEL_COUNT);

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

			synthesizer->add_sound_component_on_polyphonic_circuit(pm1);
			synthesizer->add_sound_component_on_polyphonic_circuit(pm2);
			synthesizer->add_sound_component_on_polyphonic_circuit(pm3);
			synthesizer->add_sound_component_on_polyphonic_circuit(pm4);

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

			gui_sound_component *gpm1 = new gui_sound_component(pm1, synthesizer_mutex, 500, 500);
			gui_sound_component *gpm2 = new gui_sound_component(pm2, synthesizer_mutex, 500, 500);
			gui_sound_component *gpm3 = new gui_sound_component(pm3, synthesizer_mutex, 500, 500);
			gui_sound_component *gpm4 = new gui_sound_component(pm4, synthesizer_mutex, 500, 500);

			gui_polyphonic_circuit *gui_poly = new gui_polyphonic_circuit(&m_main_factory, synthesizer, synthesizer_mutex, 0, 0, 800, 800);

			gui_poly->add_gui_component(pc);
			gui_poly->add_gui_component(pcc);
			gui_poly->add_gui_component(pccc);
			gui_poly->add_gui_component(pc2);
			gui_poly->add_gui_component(pc21);
			gui_poly->add_gui_component(pc22);
			gui_poly->add_gui_component(pc23);
			gui_poly->add_gui_component(pgf1);
			gui_poly->add_gui_component(pgf2);
			gui_poly->add_gui_component(ppc2);
			gui_poly->add_gui_component(ppc21);

			gui_poly->add_gui_component(gpm1);
			gui_poly->add_gui_component(gpm2);
			gui_poly->add_gui_component(gpm3);
			gui_poly->add_gui_component(gpm4);

			/////////

			View::page_container *pages = new View::page_container(120, 0, 800, 800, View::cl_chartreuse);

			pages->add_page(gui_master);
			pages->add_page(gui_poly);
			page_id = 0;
			pages->select_page(page_id);

			add_widget(pages);

			add_widget(new View::push_button([&, pages]
			{
				page_id = (page_id == 0) ? 1 : 0;
				pages->select_page(page_id);
			}
			, "Change page", 705, 0));

			////////////

			

			m_plugin_list_box = 
				new View::list_box(
					0, 0, 120, 800, 40, 
					[&, gui_poly, gui_master](unsigned int id) 
					{
						gui_poly->select_component_creation_factory_id(m_factory_ids[id]);
						gui_master->select_component_creation_factory_id(m_factory_ids[id]);
					},
					GuiProperties::list_box_selected_item_color, 
					GuiProperties::list_box_background, 
					GuiProperties::list_box_border_color, 
					GuiProperties::list_box_font_color, 
					12);

			add_widget(m_plugin_list_box);

			///////////

			init_main_factory();
		}

		synthesizer_gui::~synthesizer_gui()
		{
			// Plugin list box deleted by panel
		}

		void synthesizer_gui::add_plugin_factory(Sound::abstract_plugin_factory * factory)
		{
			m_main_factory.register_factory(factory);
			m_plugin_list_box->add_item(factory->get_name());
			m_factory_ids.push_back(factory->get_factory_id());
		}

		void synthesizer_gui::init_main_factory()
		{
			add_plugin_factory(new Sound::Builtin::sin_factory());
		}




	} /* Gui */

} /* Gammou */
