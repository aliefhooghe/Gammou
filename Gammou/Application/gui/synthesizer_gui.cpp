
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
                    m_gui_component_factory, synthesizer, synthesizer_mutex,
					0, 0, GuiProperties::main_gui_circuit_width, GuiProperties::main_gui_circuit_height);
            m_gui_master_circuit = master_circuit.get();

			auto polyphonic_circuit = 
				std::make_unique<gui_polyphonic_circuit>(
                    m_gui_component_factory, synthesizer, synthesizer_mutex,
					0, 0, GuiProperties::main_gui_circuit_width, GuiProperties::main_gui_circuit_height);
            m_gui_polyphonic_circuit = polyphonic_circuit.get();

            //  Pages

			auto pages =
				std::make_unique<View::page_container>(
					GuiProperties::main_gui_component_choice_box_width, GuiProperties::main_gui_toolbox_height, 
					GuiProperties::main_gui_circuit_width, GuiProperties::main_gui_circuit_height, View::cl_chartreuse);

			pages->add_page(std::move(master_circuit));
			pages->add_page(std::move(polyphonic_circuit));

            pages->select_page(0);
            m_pages = pages.get();
			add_widget(std::move(pages));

			// Component Choice ListBox

            auto selector =
                std::make_unique<component_selector>(
					0, GuiProperties::main_gui_toolbox_height - 2, 
					GuiProperties::main_gui_component_choice_box_width, 
					GuiProperties::main_gui_component_choice_box_height + 2, 
                    GuiProperties::main_gui_component_choice_box_item_count,
                    m_gui_component_factory);

            selector->set_value_select_event(
            [this](View::directory_view<unsigned int>&, const std::string&, const unsigned int& id)
            {
                m_gui_master_circuit->select_component_creation_factory_id(id);
                m_gui_polyphonic_circuit->select_component_creation_factory_id(id);
            });

            m_component_selector = selector.get();
            add_widget(std::move(selector));
						
			//// ToolBox

			auto tool_box =
				std::make_unique<View::panel<> >(
					0, 0,
					GuiProperties::main_gui_toolbox_width, 
					GuiProperties::main_gui_toolbox_height,
					GuiProperties::main_gui_tool_box_background);

			//	Circuit Selector

			auto circuit_selector = 
				std::make_unique<View::list_box>(
					GuiProperties::main_gui_size_unit * 2, 0, 
					GuiProperties::main_gui_size_unit * 3, GuiProperties::main_gui_size_unit, 
					2,
					GuiProperties::main_gui_list_box_selected_item_color, 
					GuiProperties::main_gui_list_box_hovered_item_color,
					GuiProperties::main_gui_list_box_background, 
					GuiProperties::main_gui_list_box_font_color, 
					GuiProperties::main_gui_component_choice_box_font_size);

			circuit_selector->add_item("Master Circuit");
			circuit_selector->add_item("Polyphonic Circuit");
			circuit_selector->select_item(0);

			circuit_selector->set_item_select_event(
                [this](View::list_box&, unsigned int id)
                {
                    m_pages->select_page(id);
                });

			tool_box->add_widget(
				std::move(circuit_selector));
			
			//	Legato - Polyphonic selector

			auto keyboard_mode_selector = 
				std::make_unique<View::list_box>(
					GuiProperties::main_gui_size_unit * 5, 0, 
					GuiProperties::main_gui_size_unit * 3, GuiProperties::main_gui_size_unit, 
					2,
					GuiProperties::main_gui_list_box_selected_item_color, 
					GuiProperties::main_gui_list_box_hovered_item_color,
					GuiProperties::main_gui_list_box_background, 
					GuiProperties::main_gui_list_box_font_color, 
					GuiProperties::main_gui_component_choice_box_font_size);

			keyboard_mode_selector->add_item("Polyphonic Keyboard");
			keyboard_mode_selector->add_item("Legato Keyboard");
			keyboard_mode_selector->select_item(0);

			keyboard_mode_selector->set_item_select_event(
                [synthesizer](View::list_box&, unsigned int id)
                {
					using mode = Sound::synthesizer::keyboard_mode;

                    if (id == 0)
						synthesizer->set_keyboard_mode(mode::POLYPHONIC);
					else
						synthesizer->set_keyboard_mode(mode::LEGATO);
                });

			tool_box->add_widget(
				std::move(keyboard_mode_selector));

			//---
			//	TODO : mieux
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

			m_master_volume = master_volume.get();
			
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

		void synthesizer_gui::init_main_factory()
		{
			// Built In Components
            m_component_selector->add_plugin_factory(std::make_unique<Sound::Builtin::sin_factory>());
            m_component_selector->add_plugin_factory(std::make_unique<Sound::Builtin::sum_component_factory>());
            m_component_selector->add_plugin_factory(std::make_unique<Sound::Builtin::product_factory>());
            m_component_selector->add_plugin_factory(std::make_unique<Sound::Builtin::lp2_factory>());
            m_component_selector->add_plugin_factory(std::make_unique<Sound::Builtin::adsr_env_factory>());
            m_component_selector->add_plugin_factory(std::make_unique<Sound::Builtin::saw_factory>());

            m_component_selector->add_plugin_factory(MAKE_UNIQUE_FUNCTION_COMPONENT_FACTORY(cos));
            m_component_selector->add_plugin_factory(MAKE_UNIQUE_FUNCTION_COMPONENT_FACTORY(exp));
            m_component_selector->add_plugin_factory(MAKE_UNIQUE_FUNCTION_COMPONENT_FACTORY(log));
            m_component_selector->add_plugin_factory(MAKE_UNIQUE_FUNCTION_COMPONENT_FACTORY(cosh));
            m_component_selector->add_plugin_factory(MAKE_UNIQUE_FUNCTION_COMPONENT_FACTORY(sinh));
            m_component_selector->add_plugin_factory(MAKE_UNIQUE_FUNCTION_COMPONENT_FACTORY(sqrt));
            m_component_selector->add_plugin_factory(MAKE_UNIQUE_FUNCTION_COMPONENT_FACTORY(fabs));
            m_component_selector->add_plugin_factory(MAKE_UNIQUE_FUNCTION_COMPONENT_FACTORY(atan));
            m_component_selector->add_plugin_factory(MAKE_UNIQUE_FUNCTION_COMPONENT_FACTORY(tanh));

			// Control Components
            m_component_selector->add_control_factory(std::make_unique<value_knob_gui_component_factory>());
            m_component_selector->add_control_factory(std::make_unique<gain_knob_gui_component_factory>());

            m_component_selector->add_control_factory(std::make_unique<value_slider_gui_component_factory>());
            m_component_selector->add_control_factory(std::make_unique<gain_slider_gui_component_factory>());

            m_component_selector->add_control_factory(std::make_unique<value_integer_gui_component_factory>());
            m_component_selector->add_control_factory(std::make_unique<gain_integer_gui_component_factory>());

            m_component_selector->add_control_factory(std::make_unique<user_gui_component_factory>(m_gui_component_factory));

			// Plugins Components
			const std::string plugin_dir_path(GAMMOU_PLUGINS_DIRECTORY_PATH);
			
			try {
				for (auto & p : std::experimental::filesystem::directory_iterator(plugin_dir_path)) {
					try {
						const std::string plugin_path(p.path().string());
						DEBUG_PRINT("Loading %s\n", plugin_path.c_str());
                        m_component_selector->load_plugin_factory(plugin_path);
					}
					catch(...){
						DEBUG_PRINT("Failed\n");
					}
				}
			}
			catch(...){
                DEBUG_PRINT("Error while listing plugin directory '%s'\n", GAMMOU_PLUGINS_DIRECTORY_PATH);
			}
		}


	} /* Gui */

} /* Gammou */
