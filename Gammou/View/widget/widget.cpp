
#include "widget.h"
#include "panel.h"
#include "../debug.h"

#define RECT_REDRAW_SHIFT 10

namespace Gammou {

	namespace View {
		widget::widget(const unsigned int x, const unsigned int y, const unsigned int width, const unsigned int height)
			: m_parent(nullptr), m_rect(x, y, width, height) 
		{
		}

		widget::widget(const rectangle & rect)
			: m_parent(nullptr), m_rect(rect)
		{
		}

		widget::~widget()
		{
			DEBUG_PRINT("Widget DTOR\n");
		}

		bool widget::on_key_up(const keycode key)
		{
			return false;
		}

		bool widget::on_key_down(const keycode key)
		{
			return false;
		}

		bool widget::on_mouse_enter(void)
		{
			return false;
		}

		bool widget::on_mouse_exit(void)
		{
			return false;
		}

		bool widget::on_mouse_move(const int x, const int y)
		{
			return false;
		}

		bool widget::on_mouse_wheel(const float distance)
		{
			return false;
		}

		bool widget::on_mouse_button_down(const mouse_button button, const int x, const int y)
		{
			return false;
		}

		bool widget::on_mouse_button_up(const mouse_button button, const int x, const int y)
		{
			return false;
		}

		bool widget::on_mouse_dbl_click(const int x, const int y)
		{
			return false;
		}

		bool widget::on_mouse_drag(const mouse_button button, const int x, const int y, const int dx, const int dy)
		{
			return false;
		}

		bool widget::on_mouse_drag_start(const mouse_button button, const int x, const int y)
		{
			return false;
		}

		bool widget::on_mouse_drag_end(const mouse_button button, const int x, const int y)
		{
			return false;
		}

		bool widget::contains(const unsigned int px, const unsigned int py) const 
		{
			return m_rect.contains(px, py);
		}

		bool widget::contains(const rectangle & rect) const
		{
			return m_rect.contains(rect);
		}

		unsigned int widget::get_x(void) const
		{
			return m_rect.x;
		}

		unsigned int widget::get_y(void) const
		{
			return m_rect.y;
		}

		unsigned int widget::get_width(void) const
		{
			return m_rect.width;
		}

		unsigned int widget::get_height(void) const
		{
			return m_rect.height;
		}

		const rectangle *widget::get_rect(void) const
		{
			return &m_rect;
		}

		void widget::set_pos(const unsigned int x, const unsigned int y)
		{
			m_rect.x = x;
			m_rect.y = y;
			redraw();
		}

		void widget::resize(const unsigned int width, const unsigned int height)
		{
			m_rect.width = width;
			m_rect.height = height;
			redraw();
		}

		void widget::set_rect(const rectangle & rect)
		{
			//	TODO lever des exceptions
			if( rect.x >= 0 && rect.y >= 0 
				&& rect.width > 0 && rect.height > 0)
				m_rect = rect;
			redraw();
		}

		void widget::redraw_parent()
		{
			if (m_parent != nullptr)
				m_parent->redraw();
		}

		panel * widget::get_parent()
		{
			return m_parent;
		}

		void widget::redraw()
		{
			rectangle rect = m_rect;

			rect.x = (rect.x > RECT_REDRAW_SHIFT) ? (rect.x - RECT_REDRAW_SHIFT) : 0;
			rect.y = (rect.y > RECT_REDRAW_SHIFT) ? (rect.y - RECT_REDRAW_SHIFT) : 0;
			rect.height += RECT_REDRAW_SHIFT * 2;
			rect.width += RECT_REDRAW_SHIFT * 2;

			if (m_parent != nullptr)
				m_parent->redraw_rect(rect);
		}



	} /* View */

} /* Gammou */