
#include "file_system_view.h"

namespace Gammou {

	namespace View {

		file_system_view::file_system_view(
			const std::string& root_path,
			const int x,
			const int y,
			const unsigned int width,
			const unsigned int height,
			const unsigned int displayed_cell_count,
			const color selected_cell_color,
			const color hovered_cell_color,
			const color background,
			const color font_color,
			const unsigned int font_size)
		:	file_system_directory_model(root_path),
			directory_view<std::string>(
				*this, x, y, width, height,
				displayed_cell_count, selected_cell_color,
				hovered_cell_color, background, font_color,
				font_size)
		{
        //    discover();
		}

		file_system_view::~file_system_view()
		{
		}


	} /* View */

} /* Gammou */
