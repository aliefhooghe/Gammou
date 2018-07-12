#ifndef GAMMOU_GUI_PROPERTIES_H_
#define GAMMOU_GUI_PROPERTIES_H_

#include "view.h"


namespace Gammou {

	namespace Gui {

		namespace GuiProperties {

			/*
			 *		Colors
			 */

			// Component maps
			const View::color background = 0x002339ff;
			const View::color linking_color = 0x1896d1ff;
			const View::color link_color = 0x439d9eff;
			const View::color highlight_socket_color = linking_color;

			// Components
			const View::color component_border = 0x205065ff;
			const View::color component_background = 0x003147ff;
			const View::color moving_component_background = 0x003147ff;

			const View::color component_input_socket_color = View::cl_firebrick;
			const View::color component_linked_input_socket_color = View::cl_red;
			const View::color component_output_socket_color = component_linked_input_socket_color;
			const View::color focused_component_output_socket_color = linking_color;

			const View::color component_font_color = 0xbec7e6ff;

			const View::color knob_off_color = component_font_color;
			const View::color knob_on_color = 0xc53a3dff;

			const View::color slider_on_color = knob_on_color;
			const View::color slider_off_color = knob_off_color;

			/* Main Gui */

			// Listboxs
			const View::color main_gui_list_box_background = component_background;
			const View::color main_gui_list_box_selected_item_color = moving_component_background;
			const View::color main_gui_list_box_border_color = component_border;	//	TODO
			const View::color main_gui_list_box_font_color = component_font_color;

			// ToolBox
			const View::color main_gui_tool_box_background = main_gui_list_box_background;
			const View::color main_gui_tool_box_border_color = main_gui_list_box_border_color;

			/*
			 *		Dimension
			 */

			// Components
			const float component_width = 90;

#ifdef _WIN32
			const float component_font_size = 11.0;
#else
			const float component_font_size = 10.0;
#endif

			const float component_rectangle_corner_radius = 5.0;

			const float component_border_width = 2.2;
			const float moving_component_border_width = 4.0;

			const float component_socket_radius = 3.0;
			const float focused_component_socket_radius = 4.0;
			
			const float link_width = 3;

			/* Main Gui */
			// Main gui : width*height = 24*12 (unit)

			const unsigned int main_gui_size_unit = 60;
			const unsigned int main_gui_width = 24 * main_gui_size_unit; //16 * main_gui_size_unit;
			const unsigned int main_gui_height = 12 * main_gui_size_unit;

			const unsigned int main_gui_toolbox_height = 1 * main_gui_size_unit;
			const unsigned int main_gui_toolbox_width = main_gui_width;

			const unsigned int main_gui_component_choice_box_width = 2 * main_gui_size_unit;
			const unsigned int main_gui_component_choice_box_height = main_gui_height - main_gui_toolbox_height;
			
#ifdef _WIN32
			const unsigned int main_gui_component_choice_box_font_size = 12;
#else
			const unsigned int main_gui_component_choice_box_font_size = 11;
#endif

			const unsigned int main_gui_component_choice_box_item_count = 40;

			const unsigned int main_gui_circuit_width = main_gui_width - main_gui_component_choice_box_width;
			const unsigned int main_gui_circuit_height = main_gui_height - main_gui_toolbox_height;
			
		}
	}
} /* Gammou */

#endif
