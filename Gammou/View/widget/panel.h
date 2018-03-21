
#ifndef GAMMOU_PANNEL_H_
#define GAMMOU_PANNEL_H_

#include <deque>
#include <type_traits>
#include <algorithm>
#include "widget.h"



namespace Gammou {

	namespace View {

		class abstract_panel : public widget {

		public:
			abstract_panel(const int x, const int y, const unsigned int width, const unsigned int height, 
				const color background);
			abstract_panel(const rectangle& rect, const color background);

			virtual ~abstract_panel() {}
			virtual void redraw_rect(const rectangle& rect);

			void set_background_color(const color c);
			color get_background_color(void) const;

		protected:
			virtual void draw_background(cairo_t *cr);

			void get_ownership(widget *child);
			void release_widget(widget *child);

		private:
			color m_background_color;

		};

		template<class widget_type = widget>
		class panel : public abstract_panel {

		public:
			static_assert(std::is_base_of<widget, widget_type>::value, "widget_type must inherit from widget");

			panel(const int x, const int y, const unsigned int width, const unsigned int height, 
				const color background = cl_white);
			panel(const rectangle& rect, const color background = cl_white);

			virtual ~panel();

			// widget override
			virtual bool on_key_up(const keycode key) override;
			virtual bool on_key_down(const keycode key) override;
			virtual bool on_mouse_enter(void) override;
			virtual bool on_mouse_exit(void) override;
			virtual bool on_mouse_move(const int x, const int y) override;
			virtual bool on_mouse_button_down(const mouse_button button, const int x, const int y) override;
			virtual bool on_mouse_button_up(const mouse_button button, const int x, const int y) override;
			virtual bool on_mouse_dbl_click(const int x, const int y) override;
			virtual bool on_mouse_wheel(const float distance) override;

			virtual bool on_mouse_drag_start(const mouse_button button, const int x, const int y) override;
			virtual bool on_mouse_drag(const mouse_button button, const int x, const int y,
				const int dx, const int dy) override;
			virtual bool on_mouse_drag_end(const mouse_button button, const int x, const int y) override;

			virtual void draw(cairo_t *cr) override;
		
			virtual void add_widget(widget_type *w);		//	Panel get widget ownership and have to destroy it unless it is removed
			virtual void remove_widget(widget_type *w);
		protected:
			virtual void draw_widgets(cairo_t *cr);

			widget_type *get_focused_widget(void) const;	
			widget_type *get_draging_widget(void) const;	
			widget_type *get_draging_widget(mouse_button& button) const;	
			
			std::deque<widget_type*> m_widgets;		

		private:
			widget_type *get_widget_at_position(const int x, const int y) const;

			widget_type *m_focused_widget;	//	under cursor
			widget_type *m_draging_widget;
			mouse_button m_draging_button;
		};

		template<class widget_type = widget>
		class border_panel : public panel<widget_type> {

		public:
			border_panel(const int x, const int y, const unsigned int width, const unsigned int height,
				const color background = cl_white, const color border_color = cl_black, const float border_width = 2.0f);
			border_panel(const rectangle& rect, 
				const color background = cl_white, const color border_color = cl_black, const float border_width = 2.0f);

			void set_border_color(const color c);
			color get_border_color(void) const;

		protected:
			virtual void draw_background(cairo_t *cr) override;

		private:
			const color m_border_color;
			const float m_border_width;
		};

		//////////////////////
		
		// Panel implementation

		template<class widget_type>
		panel<widget_type>::panel(
			const int x, const int y, 
			const unsigned int width, const unsigned int height, 
			const color background)
			: abstract_panel(x, y, width, height, background),
			m_focused_widget(nullptr),
			m_draging_widget(nullptr),
			m_draging_button(mouse_button::LeftButton)
		{

		}

		template<class widget_type>
		 panel<widget_type>::panel(const rectangle & rect, const color background)
			: abstract_panel(rect, background)
		{
		}

		template<class widget_type>
		 panel<widget_type>::~panel()
		{
			for (widget_type *w : m_widgets)
				delete w;
		}

		template<class widget_type>
		 bool panel<widget_type>::on_key_up(const keycode key)
		{
			if (m_focused_widget != nullptr)
				return m_focused_widget->on_key_up(key);
			else	
				return false;	//	no need to be redrawn
		}

		template<class widget_type>
		 bool panel<widget_type>::on_key_down(const keycode key)
		{
			if (m_focused_widget != nullptr)
				return m_focused_widget->on_key_down(key);
			else
				return false;
		}

		template<class widget_type>
		 bool panel<widget_type>::on_mouse_enter(void)
		{
			return false;
		}

		template<class widget_type>
		 bool panel<widget_type>::on_mouse_exit(void)
		{
			bool ret;

			if (m_focused_widget != nullptr) {
				ret = m_focused_widget->on_mouse_exit();
				m_focused_widget = nullptr;
			}

			if (m_draging_widget != nullptr) {
				ret |= m_draging_widget->on_mouse_drag_end(m_draging_button, 0, 0);
				m_draging_widget = nullptr;
			}

			return ret;
		}

		template<class widget_type>
		 bool panel<widget_type>::on_mouse_move(const int x, const int y)
		{
			widget_type *const w = get_widget_at_position(x, y);

			if (m_focused_widget == nullptr) {
				if (w != nullptr) {
					m_focused_widget = w;
					return w->on_mouse_enter();
				}
				else {
					return false;
				}
			}
			else if( m_focused_widget == w ){
				return m_focused_widget->on_mouse_move(
					x - m_focused_widget->get_x(),
					y - m_focused_widget->get_y());
			}
			else {
				bool ret = m_focused_widget->on_mouse_exit();

				if (w != nullptr)
					ret |= w->on_mouse_enter();

				m_focused_widget = w;
				return ret;
			}

		}

		template<class widget_type>
		 bool panel<widget_type>::on_mouse_button_down(const mouse_button button, const int x, const int y)
		{
			if (m_focused_widget != nullptr)
				return m_focused_widget->on_mouse_button_down(button,
					x - m_focused_widget->get_x(),
					y - m_focused_widget->get_y());
			else
				return false;
		}

		template<class widget_type>
		 bool panel<widget_type>::on_mouse_button_up(const mouse_button button, const int x, const int y)
		{
			if (m_focused_widget != nullptr)
				return m_focused_widget->on_mouse_button_up(button,
					x - m_focused_widget->get_x(),
					y - m_focused_widget->get_y());
			else
				return false;
		}

		template<class widget_type>
		 bool panel<widget_type>::on_mouse_dbl_click(const int x, const int y)
		{
			if (m_focused_widget != nullptr)
				return m_focused_widget->on_mouse_dbl_click(
					x - m_focused_widget->get_x(),
					y - m_focused_widget->get_y());
			else
				return false;
		}

		template<class widget_type>
		 bool panel<widget_type>::on_mouse_wheel(const float distance)
		{
			if (m_focused_widget != nullptr)
				return m_focused_widget->on_mouse_wheel(distance);
			else
				return false;
		}

		template<class widget_type>
		 void panel<widget_type>::draw_widgets(cairo_t * cr)
		{
			for (widget *w : m_widgets) {
				cairo_save(cr);
				cairo_translate(cr, w->get_x(), w->get_y());
				w->draw(cr);
				cairo_restore(cr);
			}
		}

		template<class widget_type>
		 void panel<widget_type>::draw(cairo_t * cr)
		{
			DEBUG_PRINT("Panel Draw\n");
			draw_background(cr);
			draw_widgets(cr);
		}

		template<class widget_type>
		bool panel<widget_type>::on_mouse_drag_start(const mouse_button button, const int x, const int y)
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

		template<class widget_type>
		 bool panel<widget_type>::on_mouse_drag(const mouse_button button, const int x, const int y, const int dx, const int dy)
		{
			widget_type *w = get_widget_at_position(x, y);
			bool ret = false;

			if (m_focused_widget != w) {
				if (m_focused_widget != nullptr)
					ret = m_focused_widget->on_mouse_exit();
				if (w != nullptr)
					ret |= w->on_mouse_enter();
				m_focused_widget = w;
			}

			if (m_draging_widget != nullptr)
				ret |= m_draging_widget->on_mouse_drag(button,
					x - m_draging_widget->get_x(),
					y - m_draging_widget->get_y(),
					dx, dy);

			return ret;
		}

		template<class widget_type>
		 bool panel<widget_type>::on_mouse_drag_end(const mouse_button button, const int x, const int y)
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

		template<class widget_type>
		 void panel<widget_type>::add_widget(widget_type * w)
		{
			if (w == nullptr)
				return;
			m_widgets.push_back(w);
			get_ownership(w);
			redraw();
		}

		template<class widget_type>
		void panel<widget_type>::remove_widget(widget_type * w)
		{
			release_widget(w);
			m_widgets.erase(std::remove(m_widgets.begin(), m_widgets.end(), w), m_widgets.end());

			if (m_focused_widget == w)
				m_focused_widget = nullptr;
			if (m_draging_widget == w)
				m_draging_widget = nullptr;

			redraw();
		}

		template<class widget_type>
		 widget_type * panel<widget_type>::get_widget_at_position(const int x, const int y) const
		{
			for (auto it = m_widgets.rbegin(); it != m_widgets.rend(); ++it) {
				widget_type *const w = (*it);
				if (w->contains(x - w->get_x(), y - w->get_y()))
					return w;
			}

			return nullptr;
		}

		template<class widget_type>
		 widget_type * panel<widget_type>::get_focused_widget(void) const
		{
			return m_focused_widget;
		}

		template<class widget_type>
		 widget_type * panel<widget_type>::get_draging_widget(void) const
		{
			return m_draging_widget;
		}

		template<class widget_type>
		 widget_type * panel<widget_type>::get_draging_widget(mouse_button & button) const
		{
			button = m_draging_button;
			return m_draging_widget;
		}

		 /////////////////////////////////////////
		 
		 // border Panel Implementaation

		 template<class widget_type>
		 border_panel<widget_type>::border_panel(
			 const int x, const int y, const unsigned int width, const unsigned int height, 
			 const color background, const color border_color, const float border_width)
			 : panel<widget_type>(x, y, width, height, background),
				m_border_color(border_color),
				m_border_width(border_width)
		 {
		 }

		 template<class widget_type>
		 border_panel<widget_type>::border_panel(const rectangle & rect, const color background, const color border_color, const float border_width)
			 : panel<widget_type>(rect, background),
				 m_border_color(border_color),
				 m_border_width(border_width)
		 {
		 }

		 template<class widget_type>
		 void border_panel<widget_type>::set_border_color(const color c)
		 {
			 if (m_border_color != c) {
				 m_border_color = c;
				 panel<widget_type>::redraw();
			 }
		 }

		 template<class widget_type>
		 color border_panel<widget_type>::get_border_color(void) const
		 {
			 return m_border_color;
		 }

		 template<class widget_type>
		 void border_panel<widget_type>::draw_background(cairo_t *cr)
		 {
			 const float pos = 0.5f * m_border_width;

			 panel<widget_type>::draw_background(cr);
			 
			 cairo_rectangle(
				 cr, pos, pos, 
				 static_cast<double>(panel<widget_type>::get_width()) - m_border_width,
				 static_cast<double>(panel<widget_type>::get_height()) - m_border_width);

			 cairo_helper::set_source_color(cr, m_border_color);
			 cairo_set_line_width(cr, m_border_width);
			 cairo_stroke(cr);
		 }

} /* View */
} /* Gammou */

#endif
