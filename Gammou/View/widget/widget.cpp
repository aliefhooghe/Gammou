
#include "widget.h"
#include "panel.h"
#include "../../debug.h"

namespace Gammou {

	namespace View {
		widget::widget(const int x, const int y, const unsigned int width, const unsigned int height)
			: m_parent(nullptr), m_absolute_rect(x, y, width, height) 
		{
		}

		widget::widget(const rectangle & rect)
			: m_parent(nullptr), m_absolute_rect(rect)
		{
		}

		widget::~widget()
		{
			//DEBUG_PRINT("Widget DTOR\n");
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

		bool widget::contains(const int px, const int py) const 
		{
			return get_relative_rect().contains(px, py);
		}

		bool widget::contains(const rectangle & rect) const
		{
			return get_relative_rect().contains(rect);
		}

		int widget::get_x(void) const
		{
			return m_absolute_rect.x;
		}

		int widget::get_y(void) const
		{
			return m_absolute_rect.y;
		}

		unsigned int widget::get_width(void) const
		{
			return m_absolute_rect.width;
		}

		unsigned int widget::get_height(void) const
		{
			return m_absolute_rect.height;
		}

		const rectangle & widget::get_absolute_rect() const
		{
			return m_absolute_rect;
		}

		void widget::set_pos(const int x, const int y)
		{
			m_absolute_rect.x = x;
			m_absolute_rect.y = y;
            redraw_parent();
		}

		void widget::resize(const unsigned int width, const unsigned int height)
		{
			m_absolute_rect.width = width;
			m_absolute_rect.height = height;
            redraw_parent();
		}

		void widget::set_rect(const rectangle & rect)
		{
			if (rect.width > 0 && rect.height > 0)
				m_absolute_rect = rect;
			else
				throw std::domain_error("Invalid widget rect");
            redraw_parent();
		}

		abstract_display *widget::get_display()
		{
			if (m_parent != nullptr)
				return m_parent->get_display();
			else
				return nullptr;
		}

		void widget::redraw_parent()
		{
			if (m_parent != nullptr)
				m_parent->redraw();
		}

		abstract_panel * widget::get_parent()
		{
			return m_parent;
		}

		const rectangle widget::get_relative_rect() const
		{
			return rectangle(0, 0, get_width(), get_height());
		}

		void widget::redraw()
		{
			if (m_parent != nullptr)
				m_parent->redraw_rect(m_absolute_rect);
		}



	} /* View */

} /* Gammou */
