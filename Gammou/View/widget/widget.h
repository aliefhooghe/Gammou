#ifndef GAMMOU_WIDGET_H_
#define GAMMOU_WIDGET_H_

#include "../cairo_definition.h"
#include "event.h"
#include "cairo_helper.h"

namespace Gammou {

	namespace View {
	

		class widget {

			//	a panel is a widget-container widget
			friend class panel;

		public:
			widget(const unsigned int x, const unsigned int y, const unsigned int width, const unsigned int height);
			widget(const rectangle& rect);
			virtual ~widget();

			unsigned int get_x(void) const;
			unsigned int get_y(void) const;
			unsigned int get_width(void) const;
			unsigned int get_height(void) const;
			const rectangle *get_rect(void) const;

			virtual bool contains(const unsigned int px, const unsigned int py) const;
			virtual bool contains(const rectangle& rect) const;

			virtual void redraw();

		protected:
			virtual bool on_key_up(const keycode key);
			virtual bool on_key_down(const keycode key);

			virtual bool on_mouse_enter(void);
			virtual bool on_mouse_exit(void);
			virtual bool on_mouse_move(const int x, const int y);
			virtual bool on_mouse_wheel(const float distance);

			virtual bool on_mouse_button_down(const mouse_button button, const int x, const int y);
			virtual bool on_mouse_button_up(const mouse_button button, const int x, const int y);
			virtual bool on_mouse_dbl_click(const int x, const int y);

			virtual bool on_mouse_drag(const mouse_button button, const int x, const int y, 
				const int dx, const int dy);
			virtual bool on_mouse_drag_start(const mouse_button button, const int x, const int y);
			virtual bool on_mouse_drag_end(const mouse_button button, const int x, const int y);

			virtual void draw(cairo_t *cr) = 0;

			virtual void set_pos(const unsigned int x, const unsigned int y);
			virtual void resize(const unsigned int width, const unsigned int height);
			virtual void set_rect(const rectangle& rect);

			void redraw_parent();
			panel *get_parent();
			
		private:
			panel *m_parent;
			rectangle m_rect;
		};

	} /* View */

}	/* Gammou */

#endif