#ifndef GAMMOU_GUI_PROPERTIES_H_
#define GAMMOU_GUI_PROPERTIES_H_

#include "view.h"


namespace Gammou {

	namespace Gui {

		namespace GuiProperties {


        void apply(View::knob& k);
        // TODO

			/*
			 *		Colors
			 */

			// Component maps
        constexpr View::color background = 0x272829ff;
        constexpr View::color linking_color = 0xe89131ff;
        constexpr View::color link_color = 0x17d563ff;
        constexpr View::color highlight_socket_color = linking_color;

        // Components
        constexpr View::color component_border = 0x20cae5ff;
        constexpr View::color component_background = 0x313234ff;
        constexpr View::color moving_component_background = component_background;

        constexpr View::color component_input_socket_color = link_color;
        constexpr View::color component_linked_input_socket_color = link_color;
        constexpr View::color component_output_socket_color = component_linked_input_socket_color;
        constexpr View::color focused_component_output_socket_color = linking_color;

        constexpr View::color component_font_color = 0xbec7e6ff;

        constexpr View::color knob_off_color = component_font_color;
        constexpr View::color knob_on_color = 0xbd63c5ff;

        constexpr View::color slider_on_color = knob_on_color;
        constexpr View::color slider_off_color = knob_off_color;

        /* Main Gui */

        // Listboxs
        constexpr View::color main_gui_list_box_background = component_background;
        constexpr View::color main_gui_list_box_selected_item_color = 0x4a4d50ff;
        constexpr View::color main_gui_list_box_hovered_item_color = 0x54575aff;
        constexpr View::color main_gui_list_box_font_color = component_font_color;

        // ToolBox
        constexpr View::color main_gui_tool_box_background = component_background; // todo

        // User Component Editor

        constexpr View::color user_component_editor_background = component_background;

        /*
			 *		Dimension
			 */

        // Components
        constexpr float component_width = 90;

#ifdef _WIN32
            constexpr float component_font_size = 11.0;
#else
            constexpr float component_font_size = 10.0;
#endif

            constexpr float component_rectangle_corner_radius = 6.5;

            constexpr float component_border_width = 1.5f;
            constexpr float moving_component_border_width = 3.0;

            constexpr float component_socket_radius = 3.0;
            constexpr float focused_component_socket_radius = 4.0;

            constexpr float link_width = 2.5;

			/* Main Gui */
			// Main gui : width*height = 24*12 (unit)

            constexpr unsigned int main_gui_size_unit = 60;
            constexpr unsigned int main_gui_width = 24 * main_gui_size_unit;
            constexpr unsigned int main_gui_height = 12 * main_gui_size_unit;

            constexpr unsigned int main_gui_toolbox_height = 1 * main_gui_size_unit;
            constexpr unsigned int main_gui_toolbox_width = main_gui_width;

            constexpr unsigned int main_gui_component_choice_box_width = 2 * main_gui_size_unit;
            constexpr unsigned int main_gui_component_choice_box_height = main_gui_height - main_gui_toolbox_height;

#ifdef _WIN32
            constexpr unsigned int main_gui_component_choice_box_font_size = 12;
#else
            constexpr unsigned int main_gui_component_choice_box_font_size = 11;
#endif

            constexpr unsigned int main_gui_component_choice_box_item_count = 30;

            constexpr unsigned int main_gui_circuit_width = main_gui_width - main_gui_component_choice_box_width;
            constexpr unsigned int main_gui_circuit_height = main_gui_height - main_gui_toolbox_height;

            // User Component Editor

            constexpr unsigned int user_component_editor_width = main_gui_circuit_width;
            constexpr unsigned int user_component_editor_height = main_gui_circuit_height;

            constexpr unsigned int user_component_editor_toolbox_height = main_gui_size_unit;
            constexpr unsigned int user_component_editor_circuit_width = user_component_editor_width;
            constexpr unsigned int user_component_editor_circuit_height = user_component_editor_height - user_component_editor_toolbox_height;


		}
	}
} /* Gammou */

#endif
