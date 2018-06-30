
#ifndef WINDOW_WIDGET_H_
#define WINDOW_WIDGET_H_

#include "panel.h"

namespace Gammou {

	namespace View {

		class window_widget : public panel<widget> {
			
		public:
			window_widget(
				const unsigned int width,
				const unsigned int height,
				const color background = cl_white);
			~window_widget();
		};

	} /* View */

} /* Gammou */

#endif