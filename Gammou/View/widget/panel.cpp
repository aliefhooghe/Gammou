
#include <algorithm>
#include "../../debug.h"
#include "panel.h"

namespace Gammou {

	namespace View {
	
		abstract_panel::abstract_panel(const unsigned int x, const unsigned int y, const unsigned int width, const unsigned int height, const color background)
			: widget(x, y, width, height),
			m_background_color(background)
		{}

		abstract_panel::abstract_panel(const rectangle & rect, const color background)
			: widget(rect),
			m_background_color(background)
		{
		}

		void abstract_panel::redraw_rect(const rectangle & rect)
		{
			abstract_panel *parent = get_parent();

			if (parent != nullptr)
				parent->redraw_rect(rect.translate(get_x(), get_y()));
		}


		void abstract_panel::set_background_color(const color c)
		{
			m_background_color = c;
			redraw();
		}

		color abstract_panel::get_background_color(void)
		{
			return m_background_color;
		}



	} /* View */
} /* Gammou */