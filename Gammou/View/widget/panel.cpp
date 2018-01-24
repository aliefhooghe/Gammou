
#include <algorithm>
#include "../../debug.h"
#include "panel.h"

namespace Gammou {

	namespace View {
	
		abstract_panel::abstract_panel(const unsigned int x, const unsigned int y, const unsigned int width, const unsigned int height, const color background)
			: widget(x, y, width, height),
			m_background_color(background),
			m_focused_widget(nullptr),
			m_draging_widget(nullptr),
			m_draging_button(mouse_button::LeftButton)
		{}

		abstract_panel::abstract_panel(const rectangle & rect, const color background)
			: widget(rect),
			m_background_color(background),
			m_focused_widget(nullptr),
			m_draging_widget(nullptr),
			m_draging_button(mouse_button::LeftButton)
		{
		}

		void abstract_panel::redraw_rect(rectangle & rect)
		{
			abstract_panel *parent = get_parent();

			if (parent != nullptr)
				parent->redraw_rect(rect);
		}

		bool abstract_panel::on_key_up(const keycode key)
		{
			if (m_focused_widget != nullptr)
				return m_focused_widget->on_key_up(key);
			else
				return false;
		}

		bool abstract_panel::on_key_down(const keycode key)
		{
			if (m_focused_widget != nullptr)
				return m_focused_widget->on_key_down(key);
			else
				return false;
		}

		bool abstract_panel::on_mouse_enter(void)
		{
			return false;
		}

		bool abstract_panel::on_mouse_exit(void)
		{
			if (m_focused_widget != nullptr) {
				m_focused_widget->on_mouse_exit();
				m_focused_widget = nullptr;
			}

			if (m_draging_widget != nullptr) {
				m_draging_widget->on_mouse_drag_end(m_draging_button, 0, 0);
				m_draging_widget = nullptr;
			}

			return false;
		}

		bool abstract_panel::on_mouse_move(const int x, const int y)
		{
			widget *w = get_widget_at_position(x, y);
			
			bool ret = false;

			if (m_focused_widget != w) {
				if (m_focused_widget != nullptr)
					ret = m_focused_widget->on_mouse_exit();
				if (w != nullptr)
					ret |= w->on_mouse_enter();
			}
			else if (m_focused_widget != nullptr) {
				ret = m_focused_widget->on_mouse_move(
					x - m_focused_widget->get_x(),
					y - m_focused_widget->get_y());
			}

			m_focused_widget = w;
			redraw();
			return false;
		}

		bool abstract_panel::on_mouse_button_down(const mouse_button button, const int x, const int y)
		{
			if (m_focused_widget != nullptr)
				return m_focused_widget->on_mouse_button_down(button, 
					x - m_focused_widget->get_x(),
					y - m_focused_widget->get_y());
			else
				return false;
		}

		bool abstract_panel::on_mouse_button_up(const mouse_button button, const int x, const int y)
		{
			if (m_focused_widget != nullptr)
				return m_focused_widget->on_mouse_button_up(button,
					x - m_focused_widget->get_x(),
					y - m_focused_widget->get_y());
			else
				return false;
		}

		bool abstract_panel::on_mouse_dbl_click(const int x, const int y)
		{	
			if (m_focused_widget != nullptr)
				return m_focused_widget->on_mouse_dbl_click(
					x - m_focused_widget->get_x(),
					y - m_focused_widget->get_y());
			else
				return false;
		}

		bool abstract_panel::on_mouse_wheel(const float distance)
		{
			if (m_focused_widget != nullptr)
				return m_focused_widget->on_mouse_wheel(distance);
			else
				return false;
		}

		bool abstract_panel::on_mouse_drag_start(const mouse_button button, const int x, const int y)
		{
			m_draging_button = button;

			if (m_focused_widget != nullptr) {
				m_draging_widget = m_focused_widget;
				return m_draging_widget->on_mouse_drag_start(button,
					x - m_draging_widget->get_x(),
					y - m_draging_widget->get_y());
			}
			else {
				return false;
			}
		}

		bool abstract_panel::on_mouse_drag(const mouse_button button, const int x, const int y, const int dx, const int dy)
		{
			widget *w = get_widget_at_position(x, y);
			bool ret = false;

			if (m_focused_widget != w) {
				if (m_focused_widget != nullptr)
					ret = m_focused_widget->on_mouse_exit();
				if (w != nullptr)
					ret |= w->on_mouse_enter();
				m_focused_widget = get_widget_at_position(x, y);
			}

			if (m_draging_widget != nullptr)
				ret |= m_draging_widget->on_mouse_drag(button,
					x - m_draging_widget->get_x(),
					y - m_draging_widget->get_y(),
					dx, dy);

			return ret;
		}

		bool abstract_panel::on_mouse_drag_end(const mouse_button button, const int x, const int y)
		{
			if (m_draging_widget != nullptr) {
				bool ret = m_draging_widget->on_mouse_drag_end(button,
					x - m_draging_widget->get_x(),
					y - m_draging_widget->get_y());
				m_draging_widget = nullptr;
				return ret;
			}
			else {
				return false;
			}

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

		widget * abstract_panel::get_focused_widget(void) const
		{
			return m_focused_widget;
		}

		widget * abstract_panel::get_draging_widget(void) const
		{
			return m_draging_widget;
		}



	} /* View */
} /* Gammou */