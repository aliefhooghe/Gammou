
#include <algorithm>
#include "../../debug.h"
#include "panel.h"

namespace Gammou {

	namespace View {
	

		panel::panel(const unsigned int x, const unsigned int y, const unsigned int width, const unsigned int height, const color background)
			: widget(x, y, width, height), m_focused_widget(nullptr), 
			m_draging_widget(nullptr), m_draging_button(mouse_button::LeftButton), m_background_color(background)
		{
		}

		panel::panel(const rectangle & rect, const color c)
			: widget(rect), m_focused_widget(nullptr), m_draging_widget(nullptr), m_background_color(c)
		{
		}

		panel::~panel()
		{
			for (widget *w : m_widgets)
				delete w;
		}

		void panel::draw(cairo_t * cr)
		{
			//	Draw Background
			cairo_helper::set_source_color(cr, m_background_color);
			cairo_rectangle(cr, 0, 0, get_width(), get_height());
			cairo_fill(cr);
			
			//	Draw widgets
			for (widget *w : m_widgets) {
				cairo_save(cr);
				cairo_translate(cr, w->get_x(), w->get_y());
				w->draw(cr);
				cairo_restore(cr);
			}
		}

		void panel::redraw_rect(rectangle & rect)
		{
			if (m_parent != nullptr)
				m_parent->redraw_rect(rect);
		}

		void panel::add_widget(widget * w)
		{
			if (w == nullptr)
				return;
			m_widgets.push_back(w);
			w->m_parent = this;
			redraw();
		}

		void panel::remove_widget(widget * w)
		{	
			m_widgets.erase(std::remove(m_widgets.begin(), m_widgets.end(), w), m_widgets.end());

			if (m_focused_widget == w)
				m_focused_widget = nullptr;
			if (m_draging_widget == w)
				m_draging_widget = nullptr;

			redraw();
		}

		void panel::set_background_color(const color c)
		{
			m_background_color = c;
			redraw();
		}

		widget * panel::widget_at_position(const unsigned int x, const unsigned int y)
		{
			for (widget *w : m_widgets) {
				if (w->contains(x, y))
					return w;
			}
			return nullptr;
		}

		widget * panel::focused_widget(void)
		{
			return m_focused_widget;
		}


		bool panel::on_key_up(const keycode key)
		{
			if (m_focused_widget != nullptr)
				return m_focused_widget->on_key_up(key);
			else
				return false;
		}

		bool panel::on_key_down(const keycode key)
		{
			if (m_focused_widget != nullptr)
				return m_focused_widget->on_key_down(key);
			else
				return false;
		}

		bool panel::on_mouse_enter(void)
		{
			return false;
		}

		bool panel::on_mouse_exit(void)
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

		bool panel::on_mouse_move(const int x, const int y)
		{
			widget *w = widget_at_position(x, y);
			bool ret = false;

			if (m_focused_widget != w) {
				if (m_focused_widget != nullptr)
					ret = m_focused_widget->on_mouse_exit();
				if (w != nullptr)
					ret |= w->on_mouse_enter();
			}
			else if( m_focused_widget != nullptr) {
				ret = m_focused_widget->on_mouse_move(
					x - m_focused_widget->get_x(), 
					y - m_focused_widget->get_y());
			}

			m_focused_widget = w;
			return false;
		}

		bool panel::on_mouse_button_down(const mouse_button button, const int x, const int y)
		{
			if (m_focused_widget != nullptr)
				return m_focused_widget->on_mouse_button_down(button, x, y);
			else
				return false;
		}

		bool panel::on_mouse_button_up(const mouse_button button, const int x, const int y)
		{
			if (m_focused_widget != nullptr)
				return m_focused_widget->on_mouse_button_up(button, 
					x - m_focused_widget->get_x(), 
					y - m_focused_widget->get_y());
			else
				return false;
		}

		bool panel::on_mouse_dbl_click(const int x, const int y)
		{
			if (m_focused_widget != nullptr)
				return m_focused_widget->on_mouse_dbl_click(
					x - m_focused_widget->get_x(),
					y - m_focused_widget->get_y());
			else
				return false;
		}

		bool panel::on_mouse_wheel(const float distance)
		{
			if (m_focused_widget != nullptr)
				return m_focused_widget->on_mouse_wheel(distance);
			else
				return false;
		}

		bool panel::on_mouse_drag_start(const mouse_button button, const int x, const int y)
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

		bool panel::on_mouse_drag(const mouse_button button, const int x, const int y, const int dx, const int dy)
		{
			widget *w = widget_at_position(x, y);
			bool ret = false;

			if (m_focused_widget != w) {
				if (m_focused_widget != nullptr)
					ret = m_focused_widget->on_mouse_exit();
				if (w != nullptr)
					ret |= w->on_mouse_enter();
				m_focused_widget = widget_at_position(x, y);
			}

			if (m_draging_widget != nullptr)
				ret |= m_draging_widget->on_mouse_drag(button, 
					x - m_draging_widget->get_x(),
					y - m_draging_widget->get_y(),
					dx, dy);

			return ret;
		}

		bool panel::on_mouse_drag_end(const mouse_button button, const int x, const int y)
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


	} /* View */
} /* Gammou */