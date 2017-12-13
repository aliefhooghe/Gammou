#ifndef GAMMOU_PANNEL_H_
#define GAMMOU_PANNEL_H_

#include <deque>
#include "widget.h"


namespace Gammou {

	namespace View {

		class panel : public widget {

		public:
			panel(const unsigned int x, const unsigned int y, const unsigned int width, const unsigned int height, const color background = cl_white);
			panel(const rectangle& rect, const color background = cl_white);
			virtual ~panel();

			virtual void redraw_rect(rectangle& rect);
		protected:
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

			void add_widget(widget *w);
			void remove_widget(widget *w);

			void set_background_color(const color c);
		protected:
			widget *widget_at_position(const unsigned int x, const unsigned int y);
			widget *focused_widget(void);

			mouse_button m_draging_button;
		
		private:
			std::deque<widget*> m_widgets;
			widget *m_focused_widget;	//	under cursor
			widget *m_draging_widget;
			
			color m_background_color;
		};

	} /* View */
} /* Gammou */

#endif