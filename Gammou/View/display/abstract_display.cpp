
#include "abstract_display.h"

namespace Gammou {

	namespace View {
		
		abstract_display::abstract_display(widget& root_widget)
		:	abstract_panel(root_widget.get_absolute_rect()),
			m_root_widget(root_widget),
			m_scale_factor(1.0f),
			m_display_width(root_widget.get_width()),
			m_display_height(root_widget.get_height()),
			m_cursor_x(), m_cursor_y(0),
			m_is_draging(false),
			m_draging_button(mouse_button::RightButton),
			m_pressed_mouse_button_count(0)
		{
			DEBUG_PRINT("ABSTRACT DISPLAY CTOR : w = %u, h = %u \n",
				get_display_width(),
				get_display_height());
			get_ownership(m_root_widget);
		}

		abstract_display::~abstract_display()
		{
			release_widget(m_root_widget);
		}

		float abstract_display::get_scale_factor() const
		{
			return m_scale_factor;
		}

		void abstract_display::set_scale_factor(const float scale_factor)
		{
			if (m_scale_factor <= 0.0f)
				throw std::invalid_argument("a display scale factor should be >= 0");
			m_scale_factor = scale_factor;
		}

		unsigned int abstract_display::get_display_width() const
		{
			return static_cast<unsigned int>(
				static_cast<float>(
					m_root_widget.get_width() * m_scale_factor));
		}

		unsigned int abstract_display::get_display_height() const
		{
			return static_cast<unsigned int>(
				static_cast<float>(
					m_root_widget.get_height() * m_scale_factor));
		}

		abstract_display *abstract_display::get_display() 
		{
			return this;
		}

		void abstract_display::redraw_rect(const rectangle & rect)
		{
			const rectangle system_rect(
				static_cast<int>(m_scale_factor * static_cast<float>(rect.x)),
				static_cast<int>(m_scale_factor * static_cast<float>(rect.y)),
				static_cast<unsigned int>(m_scale_factor * static_cast<float>(rect.width)),
				static_cast<unsigned int>(m_scale_factor * static_cast<float>(rect.height))
			);

			sys_redraw_rect(system_rect);
		}

		void abstract_display::draw(cairo_t * cr)
		{
			m_root_widget.draw(cr);
		}

		void abstract_display::sys_draw(cairo_t * cr)
		{
			cairo_scale(cr, m_scale_factor, m_scale_factor);
			draw(cr);
		}

		bool abstract_display::sys_mouse_move(const unsigned int cx, const unsigned int cy)
		{
            const unsigned int scaled_cx =
                    static_cast<unsigned int>(static_cast<float>(cx) / m_scale_factor);
            const unsigned int scaled_cy =
                    static_cast<unsigned int>(static_cast<float>(cy) / m_scale_factor);

			bool ret;
			if (m_is_draging) {
				ret = 
					m_root_widget.on_mouse_drag(
						m_draging_button, 
						scaled_cx, scaled_cy,
						scaled_cx - m_cursor_x, 
						scaled_cy - m_cursor_y);
			}
			else if (m_pressed_mouse_button_count > 0) {
				m_is_draging = true;
				ret = 
					m_root_widget.on_mouse_drag_start(
						m_draging_button, 
						scaled_cx, 
						scaled_cy);
			}
			else {
				ret = 
					m_root_widget.on_mouse_move(scaled_cx, scaled_cy);
			}

			m_cursor_x = scaled_cx;
			m_cursor_y = scaled_cy;
			return ret;
		}

		bool abstract_display::sys_mouse_enter(void)
		{
			return m_root_widget.on_mouse_enter();
		}

		bool abstract_display::sys_mouse_exit(void)
		{
			m_pressed_mouse_button_count = 0;
			m_is_draging = false;
			return m_root_widget.on_mouse_exit();
		}

		bool abstract_display::sys_mouse_button_down(const mouse_button button)
		{
			m_pressed_mouse_button_count++;
			m_draging_button = button;
			return m_root_widget.on_mouse_button_down(
				button, m_cursor_x, m_cursor_y);
		}

		bool abstract_display::sys_mouse_button_up(const mouse_button button)
		{
			if (m_pressed_mouse_button_count > 0)
				m_pressed_mouse_button_count--;

			if (m_is_draging && button == m_draging_button) {
				m_is_draging = false;
				return m_root_widget.on_mouse_drag_end(
					button, m_cursor_x, m_cursor_y);
			}
			else {
				return m_root_widget.on_mouse_button_up(
					button, m_cursor_x, m_cursor_y);
			}
		}

		bool abstract_display::sys_mouse_wheel(const float distance)
		{
			return m_root_widget.on_mouse_wheel(distance);
		}

		bool abstract_display::sys_mouse_dbl_click(void)
		{
			return m_root_widget.on_mouse_dbl_click(m_cursor_x, m_cursor_y);
		}

		bool abstract_display::sys_key_down(const keycode key)
		{
			return m_root_widget.on_key_down(key);
		}

		bool abstract_display::sys_key_up(const keycode key)
		{
			return m_root_widget.on_key_up(key);
		}

	} /* View */

} /* Gammou */
