#ifndef FILE_SYSTEM_VIEW_H_
#define FILE_SYSTEM_VIEW_H_

#include "file_system_directory_model.h"
#include "directory_view.h"

namespace Gammou {
	namespace View {


		class file_system_view :
			private file_system_directory_model,
			public directory_view<std::string> {

			public:
				file_system_view(
					const std::string& root_path,
					const int x,
					const int y,
					const unsigned int width,
					const unsigned int height,
					const unsigned int displayed_cell_count,
					const color selected_cell_color = cl_lightgrey,
					const color hovered_cell_color = cl_lemonchiffon,
					const color background = cl_white,
					const color font_color = cl_black,
					const unsigned int font_size = 11);

				~file_system_view();
		};


	}	/*	View */
} /* Gammou */


#endif
