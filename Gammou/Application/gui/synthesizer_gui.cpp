
#include "../persistence/synthesizer_persistence.h"
#include "synthesizer_gui.h"
#include "gui_properties.h"

namespace Gammou {

	namespace Gui {

		synthesizer_gui::synthesizer_gui(Sound::synthesizer * synthesizer, std::mutex * synthesizer_mutex)
			: View::generic_window(GuiProperties::main_gui_width, GuiProperties::main_gui_height)
		{
			set_background_color(View::cl_chartreuse); // for gui debuging

			//  Synthesizer Circuits

			m_gui_master_circuit = 
				new gui_master_circuit(
					&m_complete_component_factory, synthesizer, synthesizer_mutex, 
					0, 0, GuiProperties::main_gui_circuit_width, GuiProperties::main_gui_circuit_height);

			m_gui_polyphonic_circuit = 
				new gui_polyphonic_circuit(
					&m_complete_component_factory, synthesizer, synthesizer_mutex, 
					0, 0, GuiProperties::main_gui_circuit_width, GuiProperties::main_gui_circuit_height);

			View::page_container *pages = 
				new View::page_container(
					GuiProperties::main_gui_component_choice_box_width, GuiProperties::main_gui_toolbox_height, 
					GuiProperties::main_gui_circuit_width, GuiProperties::main_gui_circuit_height, View::cl_chartreuse);

			pages->add_page(m_gui_master_circuit);
			pages->add_page(m_gui_polyphonic_circuit);
			page_id = 0;
			pages->select_page(page_id);

			add_widget(pages);

			// Component Choice ListBox

			m_plugin_list_box = 
				new View::list_box(
					0, GuiProperties::main_gui_toolbox_height - 2, 
					GuiProperties::main_gui_component_choice_box_width, 
					GuiProperties::main_gui_component_choice_box_height + 2, 
					GuiProperties::main_gui_component_choice_box_item_count,
					[&](unsigned int id) 
					{
						const unsigned int factory_id = m_factory_ids[id];
						DEBUG_PRINT("SELECT factory id %u\n", factory_id);
						m_gui_polyphonic_circuit->select_component_creation_factory_id(factory_id);
						m_gui_master_circuit->select_component_creation_factory_id(factory_id);
					},
					GuiProperties::main_gui_list_box_selected_item_color, 
					GuiProperties::main_gui_list_box_background, 
					GuiProperties::main_gui_list_box_border_color, 
					GuiProperties::main_gui_list_box_font_color, 
					GuiProperties::main_gui_component_choice_box_font_size);

			add_widget(m_plugin_list_box);
						
			// ToolBox

			View::panel<> *tool_box =
				new View::border_panel<>(
					0, 0,
					GuiProperties::main_gui_toolbox_width, GuiProperties::main_gui_toolbox_height,
					GuiProperties::main_gui_tool_box_background, GuiProperties::main_gui_tool_box_border_color);

			tool_box->add_widget(new View::push_button(
				[&, pages](View::push_button *self)
			{
				page_id = (page_id == 0) ? 1 : 0;
				pages->select_page(page_id);
			}
			, "Change page", 705, 0));

			const unsigned int offset = (GuiProperties::main_gui_size_unit - 50) / 2;

			tool_box->add_widget(new View::knob(
				[](View::knob *kn) {},
				offset + GuiProperties::main_gui_width - GuiProperties::main_gui_size_unit,
				offset,
				View::cl_firebrick,
				View::cl_lightgrey
				));

			add_widget(tool_box);

			///////////

			init_main_factory();

			// scale(0.8f); Ableton
		}

		synthesizer_gui::~synthesizer_gui()
		{
			// widgets deleted by panel
			DEBUG_PRINT("Syn Gui DTOR\n");
		}

		bool synthesizer_gui::save_state(Sound::data_sink & data)
		{
			DEBUG_PRINT("SYN SAVE STATE\n");
			m_gui_master_circuit->save_state(data);
			m_gui_polyphonic_circuit->save_state(data);
			return true;
		}

		bool synthesizer_gui::load_state(Sound::data_source & data)
		{
			DEBUG_PRINT("SYN LOAD STATE :\n");
			m_gui_master_circuit->load_state(data);
			m_gui_polyphonic_circuit->load_state(data);
			DEBUG_PRINT("SYN STATE LOADED\n");
			return true;
		}

		void synthesizer_gui::add_plugin_factory(Sound::abstract_plugin_factory * factory)
		{
			m_complete_component_factory.register_plugin_factory(factory);
			m_plugin_list_box->add_item(factory->get_name());
			m_factory_ids.push_back(factory->get_factory_id());
		}

		void synthesizer_gui::add_control_factory(complete_component_factory * factory)
		{
			m_complete_component_factory.register_complete_factory(factory);
			m_plugin_list_box->add_item(factory->get_name());
			m_factory_ids.push_back(factory->get_factory_id());
		}

		void synthesizer_gui::init_main_factory()
		{
			add_plugin_factory(new Sound::Builtin::sin_factory());
			add_plugin_factory(new Sound::Builtin::debug_factory());
			add_plugin_factory(new Sound::Builtin::product_factory());
			add_plugin_factory(new Sound::Builtin::fpb2_factory());
			add_plugin_factory(new Sound::Builtin::adsr_env_factory());
			add_plugin_factory(new Sound::Builtin::saw_factory());

			add_plugin_factory(new Sound::Builtin::perfect_saw_factory());
			add_plugin_factory(new Sound::Builtin::naive_saw_factory());
			add_plugin_factory(new Sound::Builtin::cracra_factory());

			add_control_factory(new knob_complete_component_factory());
		}




	} /* Gui */

} /* Gammou */