
#include <cstring>

#include <experimental/filesystem>
#include <filesystem>

#include "../../Synthesizer/builtin_components.h"
#include "../persistence/synthesizer_persistence.h"
#include "synthesizer_gui.h"
#include "gui_properties.h"

// for test
#include "plugin_request_dialog.h"

namespace Gammou {

	namespace Gui {

		synthesizer_gui::synthesizer_gui(
			Sound::synthesizer * synthesizer, 
			std::mutex * synthesizer_mutex)
		:	View::window_widget(
				GuiProperties::main_gui_width, 
				GuiProperties::main_gui_height,
				View::cl_chartreuse), // for gui debuging
			m_synthesizer(*synthesizer)
		{
			DEBUG_PRINT("SYN GUI CTOR\n");

			//  Synthesizer Circuits

			auto master_circuit = 
				std::make_unique<gui_master_circuit>(
					&m_gui_component_factory, synthesizer, synthesizer_mutex, 
					0, 0, GuiProperties::main_gui_circuit_width, GuiProperties::main_gui_circuit_height);
            m_gui_master_circuit = master_circuit.get();

			auto polyphonic_circuit = 
				std::make_unique<gui_polyphonic_circuit>(
					&m_gui_component_factory, synthesizer, synthesizer_mutex, 
					0, 0, GuiProperties::main_gui_circuit_width, GuiProperties::main_gui_circuit_height);
            m_gui_polyphonic_circuit = polyphonic_circuit.get();

			auto pages =
				std::make_unique<View::page_container>(
					GuiProperties::main_gui_component_choice_box_width, GuiProperties::main_gui_toolbox_height, 
					GuiProperties::main_gui_circuit_width, GuiProperties::main_gui_circuit_height, View::cl_chartreuse);
            View::page_container *pages_ptr = pages.get();


			pages->add_page(std::move(master_circuit));
			pages->add_page(std::move(polyphonic_circuit));

			page_id = 0;
			pages->select_page(page_id);

			add_widget(std::move(pages));

			// Component Choice ListBox

			auto plugin_list_box = 
				std::make_unique<View::list_box>(
					0, GuiProperties::main_gui_toolbox_height - 2, 
					GuiProperties::main_gui_component_choice_box_width, 
					GuiProperties::main_gui_component_choice_box_height + 2, 
					GuiProperties::main_gui_component_choice_box_item_count,
					GuiProperties::main_gui_list_box_selected_item_color, 
					GuiProperties::main_gui_list_box_background, 
					GuiProperties::main_gui_list_box_border_color, 
					GuiProperties::main_gui_list_box_font_color, 
					GuiProperties::main_gui_component_choice_box_font_size);

            plugin_list_box->
                set_item_select_event(
                    [&](View::list_box&, unsigned int id)
                    {
                        const unsigned int factory_id = m_factory_ids[id];
                        DEBUG_PRINT("SELECT factory id %u\n", factory_id);
                        m_gui_polyphonic_circuit->select_component_creation_factory_id(factory_id);
                        m_gui_master_circuit->select_component_creation_factory_id(factory_id);
                    });

			m_plugin_list_box = &(*plugin_list_box);
			add_widget(std::move(plugin_list_box));
						
			// ToolBox

			auto tool_box =
				std::make_unique<View::border_panel<> >(
					0, 0,
					GuiProperties::main_gui_toolbox_width, GuiProperties::main_gui_toolbox_height,
					GuiProperties::main_gui_tool_box_background, GuiProperties::main_gui_tool_box_border_color);

			tool_box->add_widget(std::make_unique<View::push_button>(
				[&, pages_ptr](View::push_button *self)
				{
					if (page_id == 0) {
						page_id = 1;
						self->set_text("Polyphonic Circuit");
					}
					else {
						page_id = 0;
						self->set_text("Master Circuit");
					}
					pages_ptr->select_page(page_id);
				}
				, "Master Circuit", 705, 16, 110, 27, 9));
			
			//---

			

			tool_box->add_widget(
				std::make_unique<View::push_button>(
					[synthesizer](View::push_button *self)
					{
						typedef Sound::synthesizer::keyboard_mode mode;

						if (synthesizer->get_keyboard_mode() == mode::POLYPHONIC) {
							synthesizer->set_keyboard_mode(mode::LEGATO);
							self->set_text("Legato");
						}
						else {	//	LEGATO
							synthesizer->set_keyboard_mode(mode::POLYPHONIC);
							self->set_text("Polyphonic");
						}
					},
					"Polyphonic", 512, 16, 110, 27, 9));

			//---

			const unsigned int offset = (GuiProperties::main_gui_size_unit - 50) / 2;

			auto master_volume = 
				std::make_unique<View::knob>(
					[synthesizer](View::knob *kn) 
					{ 
                        const float norm = kn->get_normalized_value();
                        const double volume = (expf(5.0f * norm) - 1.0) / (expf(5.0f) - 1.0);
						synthesizer->set_master_volume(volume); 
					},
					offset + GuiProperties::main_gui_width - GuiProperties::main_gui_size_unit,
					offset,
					GuiProperties::knob_on_color,
					GuiProperties::knob_off_color
				);

			m_master_volume = &(*master_volume);
			
			master_volume->set_normalized_value(1.0); // coherence with synthesizer initial value
			tool_box->add_widget(std::move(master_volume));
			add_widget(std::move(tool_box));


			init_main_factory();
			DEBUG_PRINT("SYN GUI CTOR finnished\n");
		}

		synthesizer_gui::~synthesizer_gui()
		{
			DEBUG_PRINT("Syn Gui DTOR\n");

            //  Force user gui_component deletion here in order to
            //  remove loaded sound_component to be deleted before their factory
            m_gui_master_circuit->reset_content();
            m_gui_polyphonic_circuit->reset_content();
        }

		bool synthesizer_gui::save_state(Sound::data_output_stream & data)
		{
			Persistence::synthesizer_record_header header;

			DEBUG_PRINT("SYN SAVE STATE\n");

			// Fill header
			std::memcpy(header.magic, "GAMMOU", 6);
			header.format_version_id = Persistence::gammou_format_version_id;
			header.parameter_count = GAMMOU_PARAMETER_INPUT_COUNT;

			// Save header
			if (data.write(&header, sizeof(header)) != sizeof(header))
				return false;

			// Save Parameters
			for (unsigned int i = 0; i < GAMMOU_PARAMETER_INPUT_COUNT; ++i) {
				double param_value =
					m_synthesizer.get_parameter_value(i);

				if (data.write(&param_value, sizeof(double)) != sizeof(double))
					return false;
			}

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

		bool synthesizer_gui::load_state(Sound::data_input_stream & data)
		{
			DEBUG_PRINT("SYN LOAD STATE :\n");

			// Loading header
			Persistence::synthesizer_record_header header;
			if (data.read(&header, sizeof(header)) != sizeof(header))
				return false;

			// Check magic Id
			if (std::strncmp(header.magic, "GAMMOU", 6) != 0)
				return false;

			// Check format version
			if (header.format_version_id != Persistence::gammou_format_version_id)
				return false;

			// Load Parameters
			if (header.parameter_count != GAMMOU_PARAMETER_INPUT_COUNT)
				return false;

			for (unsigned int i = 0; i < GAMMOU_PARAMETER_INPUT_COUNT; ++i) {
				double param_value;

				if (data.read(&param_value, sizeof(double)) != sizeof(double))
					return false;

				m_synthesizer.set_parameter_value(param_value, i);
			}

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
			m_gui_component_factory.register_plugin_factory(factory);
			m_plugin_list_box->add_item(factory->get_name());
			m_factory_ids.push_back(factory->get_factory_id());
		}

		void synthesizer_gui::add_control_factory(abstract_gui_component_factory * factory)
		{
			m_gui_component_factory.register_complete_factory(factory);
			m_plugin_list_box->add_item(factory->get_name());
			m_factory_ids.push_back(factory->get_factory_id());
		}

		void synthesizer_gui::load_plugin_factory(const std::string & path)
		{
			const unsigned int factory_id = m_gui_component_factory.load_plugin_factory(path);

			DEBUG_PRINT("Loaded Factory : id = %u\n", factory_id);

			m_plugin_list_box->add_item(m_gui_component_factory.get_factory_name(factory_id));
			m_factory_ids.push_back(factory_id);
		}



		void synthesizer_gui::init_main_factory()
		{

			// Built In Components
			add_plugin_factory(new Sound::Builtin::sin_factory());
			add_plugin_factory(new Sound::Builtin::sum_component_factory());
			add_plugin_factory(new Sound::Builtin::product_factory());
			add_plugin_factory(new Sound::Builtin::lp2_factory());
			add_plugin_factory(new Sound::Builtin::adsr_env_factory());
			add_plugin_factory(new Sound::Builtin::saw_factory());


			add_plugin_factory(new Sound::Builtin::function_factory<cos>("cos", 100));
			add_plugin_factory(new Sound::Builtin::function_factory<exp>("exp", 101));
			add_plugin_factory(new Sound::Builtin::function_factory<log>("log", 102));
			add_plugin_factory(new Sound::Builtin::function_factory<cosh>("cosh", 103));
			add_plugin_factory(new Sound::Builtin::function_factory<sinh>("sinh", 104));
			add_plugin_factory(new Sound::Builtin::function_factory<sqrt>("sqrt", 105));
			add_plugin_factory(new Sound::Builtin::function_factory<fabs>("abs", 106));
			add_plugin_factory(new Sound::Builtin::function_factory<atan>("atan", 107));

			// Control Components

			add_control_factory(new value_knob_gui_component_factory());
			add_control_factory(new gain_knob_gui_component_factory());

			add_control_factory(new value_slider_gui_component_factory());
			add_control_factory(new gain_slider_gui_component_factory());

			add_control_factory(new value_integer_gui_component_factory());
			add_control_factory(new gain_integer_gui_component_factory());

			// Plugins Components

			const std::string plugin_dir_path(GAMMOU_PLUGINS_DIRECTORY_PATH);
			
			try {
				for (auto & p : std::experimental::filesystem::directory_iterator(plugin_dir_path)) {
					try {
						const std::string plugin_path(p.path().string());
						DEBUG_PRINT("Loading %s\n", plugin_path.c_str());
						load_plugin_factory(plugin_path);
					}
					catch(...){
						DEBUG_PRINT("Failed\n");
					}
				}
			}
			catch(...){
				DEBUG_PRINT("Error while loding plugins in '%s'\n", GAMMOU_PLUGINS_DIRECTORY_PATH);
			}
		}


	} /* Gui */

} /* Gammou */
