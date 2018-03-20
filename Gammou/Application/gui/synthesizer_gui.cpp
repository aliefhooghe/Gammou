
#include "../persistence/synthesizer_persistence.h"
#include "synthesizer_gui.h"
#include "gui_properties.h"

namespace Gammou {

	namespace Gui {

		synthesizer_gui::synthesizer_gui(Sound::synthesizer * synthesizer, std::mutex * synthesizer_mutex)
			: View::generic_window(GuiProperties::main_gui_width, GuiProperties::main_gui_height)
		{
			DEBUG_PRINT("SYN GUI CTOR\n");
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
				if (page_id == 0) {
					page_id = 1;
					self->set_text("Polyphonic Circuit");
				}
				else {
					page_id = 0;
					self->set_text("Master Circuit");
				}
				pages->select_page(page_id);
			}
			, "Master Circuit", 705, 16, 110));

			const unsigned int offset = (GuiProperties::main_gui_size_unit - 50) / 2;

			m_master_volume = new View::knob(
				[synthesizer](View::knob *kn) 
				{ 
					const double norm = kn->get_normalized_value();
					const double volume = (expf(5.0f * norm) - 1.0) / (expf(5.0f) - 1.0);
					synthesizer->set_master_volume(volume); 
				},
				offset + GuiProperties::main_gui_width - GuiProperties::main_gui_size_unit,
				offset,
				GuiProperties::knob_on_color,
				GuiProperties::knob_off_color
			);

			m_master_volume->set_normalized_value(1.0); // coherence with synthesizer initial value
			tool_box->add_widget(m_master_volume);
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

			// Save Master Volume
			double master_volume = m_master_volume->get_normalized_value();

			if (data.write(&master_volume, sizeof(double)) != sizeof(double))
				return false;

			// Save Master Circuit
			m_gui_master_circuit->save_state(data);

			// Save Polyphonic Circuit
			m_gui_polyphonic_circuit->save_state(data);
			return true;
		}

		bool synthesizer_gui::load_state(Sound::data_source & data)
		{
			DEBUG_PRINT("SYN LOAD STATE :\n");

			// Load Master Volume
			double master_volume;
			if (data.read(&master_volume, sizeof(double)) != sizeof(double))
				return false;

			m_master_volume->set_normalized_value(master_volume);
			
			// Load Master Circuit 
			m_gui_master_circuit->load_state(data);

			// Load Polyphonic Circuit
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
			add_plugin_factory(new Sound::Builtin::sum_component_factory());
			add_plugin_factory(new Sound::Builtin::product_factory());
			add_plugin_factory(new Sound::Builtin::fpb2_factory());
			add_plugin_factory(new Sound::Builtin::adsr_env_factory());
			add_plugin_factory(new Sound::Builtin::saw_factory());

			//add_plugin_factory(new Sound::Builtin::perfect_saw_factory());
			add_plugin_factory(new Sound::Builtin::naive_saw_factory());
			//add_plugin_factory(new Sound::Builtin::cracra_factory());

			add_control_factory(new knob_complete_component_factory());
		}




	} /* Gui */

} /* Gammou */
