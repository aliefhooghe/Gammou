#ifndef GAMMOU_PANNEL_H_
#define GAMMOU_PANNEL_H_

#include <deque>
#include <type_traits>
#include "widget.h"


namespace Gammou {

	namespace View {

		class abstract_panel : public widget {
		public:
			abstract_panel(const unsigned int x, const unsigned int y, const unsigned int width, const unsigned int height, const color background);
			abstract_panel(const rectangle& rect, const color background);

			virtual ~abstract_panel() {}
			virtual void redraw_rect(rectangle& rect);

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

			void set_background_color(const color c);
			color get_background_color(void);

		protected:
			virtual widget *get_widget_at_position(const unsigned int x, const unsigned int y) const= 0;
			
			widget *get_focused_widget(void) const;
			widget *get_draging_widget(void) const;
			
		private:
			mouse_button m_draging_button;
			color m_background_color;

			widget *m_focused_widget;	//	under cursor
			widget *m_draging_widget;
		};



		template<class widget_type = widget>
		class panel : public abstract_panel {

		public:
			static_assert(std::is_base_of<widget, widget_type>::value, "widget_type must inherit from widget");

			panel(const unsigned int x, const unsigned int y, const unsigned int width,
				const unsigned int height, const color background = cl_white)
				: abstract_panel(x, y, width, height, background)
			{

			}

			panel(const rectangle& rect, const color background = cl_white)
				: abstract_panel(rect, background), 
				m_focused_widget(nullptr), 
				m_draging_widget(nullptr), 
				m_background_color(c)
			{

			}

			virtual ~panel()
			{
				for (widget *w : m_widgets)
					delete w;
			}

			virtual void redraw_rect(rectangle& rect) override
			{
				if (m_parent != nullptr)
					m_parent->redraw_rect(rect);
			}

		protected:

			virtual void draw(cairo_t *cr) override // ????
			{
				//	Draw Background
				cairo_helper::set_source_color(cr, get_background_color());
				cairo_rectangle(cr, 0, 0, get_width(), get_height());
				cairo_fill(cr);

				//	Draw widgets
				for (widget_type *w : m_widgets) {
					cairo_save(cr);
					cairo_translate(cr, w->get_x(), w->get_y());
					w->draw(cr);
					cairo_restore(cr);
				}
			}

			void add_widget(widget_type *w)
			{
				if (w == nullptr)
					return;
				m_widgets.push_back(w);
				w->m_parent = this;
				redraw();
			}

			void remove_widget(widget_type *w)
			{
				m_widgets.erase(std::remove(m_widgets.begin(), m_widgets.end(), w), m_widgets.end());

				if (m_focused_widget == w)
					m_focused_widget = nullptr;
				if (m_draging_widget == w)
					m_draging_widget = nullptr;

				redraw();
			}


		protected:
			widget_type *get_widget_at_position(const unsigned int x, const unsigned int y) const override
			{
				for (widget_type *w : m_widgets) {
					if (w->contains(x, y))
						return w;
				}

				return nullptr;
			}

			mouse_button m_draging_button;
		
		private:
			std::deque<widget_type*> m_widgets;

		};

	} /* View */
} /* Gammou */

#endif