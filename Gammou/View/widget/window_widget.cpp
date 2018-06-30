
#include "window_widget.h"

namespace Gammou {

	namespace View {

		window_widget::window_widget(
			const unsigned int width, 
			const unsigned int height,
			const color background)
		: panel<widget>(0, 0, width, height, background)
		{
		}

		window_widget::~window_widget()
		{
		}

	} /* View */

} /* Gammou */