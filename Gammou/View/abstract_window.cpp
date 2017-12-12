
#include "gammou_abstract_window.h"

namespace Gammou {

	namespace View {

		abstract_window::abstract_window(const unsigned int px_width, const unsigned int px_height)
			: panel(0, 0, px_width, px_height), 
			m_cursor_x(0), m_cursor_y(0), 
			m_is_draging(false),m_draging_button(mouse_button::RightButton), 
			m_pressed_mouse_button_count(0)
		{

		}
		
		abstract_window::~abstract_window()
		{
		}

		void abstract_window::sys_draw(cairo_t * cr)
		{
			draw(cr);
		}

		bool abstract_window::sys_mouse_move(const unsigned int cx, const unsigned int cy)
		{
			bool ret;
			if (m_is_draging) {
				ret = on_mouse_drag(m_draging_button, cx, cy, cx - m_cursor_x, cy - m_cursor_y);
			}
			else if (m_pressed_mouse_button_count > 0) {
				m_is_draging = true;
				ret = on_mouse_drag_start(m_draging_button, cx, cy);
			}
			else {
				ret = on_mouse_move(cx, cy);
			}

			m_cursor_x = cx;
			m_cursor_y = cy;
			return ret;
		}

		bool abstract_window::sys_mouse_enter(void)
		{
			return on_mouse_enter();
		}

		bool abstract_window::sys_mouse_exit(void)
		{
			m_pressed_mouse_button_count = 0;
			m_is_draging = false;
			return on_mouse_exit();
		}

		bool abstract_window::sys_mouse_button_down(const mouse_button button)
		{
			m_pressed_mouse_button_count++;
			m_draging_button = button;
			return on_mouse_button_down(button, m_cursor_x, m_cursor_y);
		}

		bool abstract_window::sys_mouse_button_up(const mouse_button button)
		{
			if (m_pressed_mouse_button_count > 0)
				m_pressed_mouse_button_count--;

			if (m_is_draging && button == m_draging_button) {
				m_is_draging = false;
				return on_mouse_drag_end(button, m_cursor_x, m_cursor_y);
			}
			else {
				return on_mouse_button_up(button, m_cursor_x, m_cursor_y);
			}
		}

		bool abstract_window::sys_mouse_wheel(const float distance)
		{
			return on_mouse_wheel(distance);
		}

		bool abstract_window::sys_mouse_dbl_click(void)
		{
			return on_mouse_dbl_click(m_cursor_x, m_cursor_y);
		}

		bool abstract_window::sys_key_down(const keycode key)
		{
			return on_key_down(key);
		}

		bool abstract_window::sys_key_up(const keycode key)
		{
			return on_key_up(key);
		}


	} /* View */

} /* Gammou */