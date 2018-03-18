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
			const View::color background =  0x323232FF;
			const View::color linking_color = 0x2DA0FFFF;
			const View::color link_color = 0x2BECC6FF;
			const View::color highlight_socket_color = linking_color;

			// Components
			const View::color component_border = 0xF7BF00FF;
			const View::color component_background = 0x464646FF;
			const View::color moving_component_background = 0x5A5A5AFF;

			const View::color component_input_socket_color = View::cl_firebrick;
			const View::color component_linked_input_socket_color = View::cl_red;
			const View::color component_output_socket_color = component_linked_input_socket_color;
			const View::color focused_component_output_socket_color = linking_color;

			const View::color component_font_color = View::cl_lightgrey;

			/* Main Gui */

			// Listboxs
			const View::color main_gui_list_box_background = component_background;
			const View::color main_gui_list_box_selected_item_color = background;
			const View::color main_gui_list_box_border_color = component_border;
			const View::color main_gui_list_box_font_color = component_font_color;

			// ToolBox
			const View::color main_gui_tool_box_background = component_background;
			const View::color main_gui_tool_box_border_color = component_border;


			/*
			 *		Dimension
			 */

			// Components
			const float component_width = 90;

			const float component_font_size = 11.0;
			const float component_rectangle_corner_radius = 5.0;
			const float component_border_width = 1.5;
			const float moving_component_border_width = 2.5;
			const float component_socket_radius = 3.0;
			const float focused_component_socket_radius = 4.0;
			
			const float link_width = 3.0;

			/* Main Gui */
			// Main gui : width*height = 16*12 (unit)

			const unsigned int main_gui_size_unit = 60;
			const unsigned int main_gui_width = 16 * main_gui_size_unit;
			const unsigned int main_gui_height = 12 * main_gui_size_unit;

			const unsigned int main_gui_toolbox_height = 1 * main_gui_size_unit;
			const unsigned int main_gui_toolbox_width = main_gui_width;

			const unsigned int main_gui_component_choice_box_width = 2 * main_gui_size_unit;
			const unsigned int main_gui_component_choice_box_height = main_gui_height - main_gui_toolbox_height;
			const unsigned int main_gui_component_choice_box_font_size = 12;
			const unsigned int main_gui_component_choice_box_item_count = 40;

			const unsigned int main_gui_circuit_width = main_gui_width - main_gui_component_choice_box_width;
			const unsigned int main_gui_circuit_height = main_gui_height - main_gui_toolbox_height;
		}

	}

} /* Gammou */

#endif