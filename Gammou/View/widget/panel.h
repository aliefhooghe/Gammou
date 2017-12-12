#ifndef GAMMOU_PANNEL_H_
#define GAMMOU_PANNEL_H_

#include <deque>
#include "widget.h"


namespace Gammou {

	namespace View {

		class panel : public widget {

		public:
			panel(const unsigned int x, const unsigned int y, const unsigned int width, const unsigned int height, const float background_red = 1.0, const float background_g = 1.0, const float background_b = 1.0);
			panel(const rectangle& rect, const float background_red = 1.0, const float background_g = 1.0, const float background_b = 1.0);
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

			void set_background_color(const float r, const float g, const float b);
		protected:
			widget *widget_at_position(const unsigned int x, const unsigned int y);
			widget *focused_widget(void);

			mouse_button m_draging_button;
		
		private:
			std::deque<widget*> m_widgets;
			widget *m_focused_widget;	//	under cursor
			widget *m_draging_widget;
			

			float m_background_r;
			float m_background_g;
			float m_background_b;
		};

	} /* View */
} /* Gammou */

#endif