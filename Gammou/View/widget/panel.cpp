
#include <algorithm>
#include "../../debug.h"
#include "panel.h"

namespace Gammou {

	namespace View {
	
		abstract_panel::abstract_panel(const int x, const int y, const unsigned int width, const unsigned int height, const color background)
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
			abstract_panel *const parent = get_parent();

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

		void abstract_panel::draw_background(cairo_t * cr)
		{
			cairo_helper::set_source_color(cr, get_background_color());
			cairo_rectangle(cr, 0, 0, get_width(), get_height());
			cairo_fill_preserve(cr);
			cairo_clip(cr); // to be sure that nothing will be drawn outside
		}

		void abstract_panel::get_ownership(widget * child)
		{
			if( child != nullptr)
				child->m_parent = this;
		}

		void abstract_panel::release_widget(widget * w)
		{
			if (w != nullptr && w->m_parent == this)
				w->m_parent = nullptr;
		}



	} /* View */
} /* Gammou */