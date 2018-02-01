#include "synthesizer_gui.h"


namespace Gammou {

	namespace Gui {

		synthesizer_gui::synthesizer_gui(Sound::synthesizer * synthesizer, std::mutex * synthesizer_mutex, unsigned int width, 
			const unsigned int height)
			: View::generic_window(width, height)
		{

			set_background_color(View::cl_chartreuse); // for gui debuging


			gui_master_circuit *gui_master = new gui_master_circuit(&m_main_factory, synthesizer, synthesizer_mutex, 0, 0, 800, 800);
			gui_polyphonic_circuit *gui_poly = new gui_polyphonic_circuit(&m_main_factory, synthesizer, synthesizer_mutex, 0, 0, 800, 800);

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
			DEBUG_PRINT("Syn Gui DTOR\n");
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
			add_plugin_factory(new Sound::Builtin::debug_factory());
		}




	} /* Gui */

} /* Gammou */
