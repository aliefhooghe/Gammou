
#include "proxy_widget.h"

namespace Gammou {

	namespace View {

		proxy_widget::proxy_widget(widget& w)
		:	abstract_panel (w.get_absolute_rect()),
			m_widget(w)
		{
            get_ownership(w);
		}

		proxy_widget::~proxy_widget()
        {
            release_widget(m_widget);
        }

		bool proxy_widget::on_key_up(const keycode key)
		{
			return m_widget.on_key_up(key);
		}

		bool proxy_widget::on_key_down(const keycode key)
		{
			return m_widget.on_key_down(key);
		}

		bool proxy_widget::on_mouse_enter(void)
		{
			return m_widget.on_mouse_enter();
		}

		bool proxy_widget::on_mouse_exit(void)
		{
			return m_widget.on_mouse_exit();
		}

		bool proxy_widget::on_mouse_move(const int x, const int y)
		{
			return m_widget.on_mouse_move(x, y);
		}

		bool proxy_widget::on_mouse_wheel(const float distance)
		{
			return m_widget.on_mouse_wheel(distance);
		}

		bool proxy_widget::on_mouse_button_down(const mouse_button button, const int x, const int y)
		{
			return m_widget.on_mouse_button_down(button, x, y);
		}

		bool proxy_widget::on_mouse_button_up(const mouse_button button, const int x, const int y)
		{
			return m_widget.on_mouse_button_up(button, x, y);
		}

		bool proxy_widget::on_mouse_dbl_click(const int x, const int y)
		{
			return m_widget.on_mouse_dbl_click(x, y);
		}

		bool proxy_widget::on_mouse_drag(const mouse_button button, const int x, const int y, const int dx, const int dy)
		{
			return m_widget.on_mouse_drag(button, x, y, dx, dy);
		}

		bool proxy_widget::on_mouse_drag_start(const mouse_button button, const int x, const int y)
		{
			return m_widget.on_mouse_drag_start(button, x, y);
		}

		bool proxy_widget::on_mouse_drag_end(const mouse_button button, const int x, const int y)
		{
			return m_widget.on_mouse_drag_end(button, x, y);
		}

		void proxy_widget::draw(cairo_t *cr)
		{
			m_widget.draw(cr);
		}

		void proxy_widget::set_pos(const int x, const int y)
		{
			m_widget.set_pos(x, y);
		}

		void proxy_widget::resize(const unsigned int width, const unsigned int height)
		{
			m_widget.resize(width, height);
		}

		void proxy_widget::set_rect(const rectangle& rect)
		{
			m_widget.set_rect(rect);
		}

	}	/* View */

} /* Gammou */
