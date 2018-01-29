
#include "view.h"


namespace Gammou {

	namespace Gui {

		namespace Palette {

			// Global
			

			// Component map
			const View::color background = 0x323232FF;
			const View::color linking_color = 0x2DA0FFFF;
			const View::color link_color = 0x2BECC6FF;
			const View::color highlight_socket_color = linking_color;

			// Component
			const View::color component_border = 0xF7BF00FF;
			const View::color component_background = 0x464646FF;
			const View::color moving_component_background = 0x5A5A5AFF;

			const View::color component_input_socket_color = View::cl_firebrick;
			const View::color component_linked_input_socket_color = View::cl_red;
			const View::color component_output_socket_color = component_linked_input_socket_color;
			const View::color component_focused_output_socket_color = linking_color;

			const View::color component_font_color = View::cl_lightgrey;


			// Listbox
			const View::color list_box_background = component_background;
			const View::color list_box_selected_item_color = background;
			const View::color list_box_border_color = component_border;
			const View::color list_box_font_color = component_font_color;
		}

	}

} /* Gammou */