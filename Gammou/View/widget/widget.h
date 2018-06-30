#ifndef GAMMOU_WIDGET_H_
#define GAMMOU_WIDGET_H_

#include <stdexcept>

#include "../cairo_definition.h"
#include "event.h"
#include "cairo_helper.h"

#include "../../debug.h"

namespace Gammou {

	namespace View {
	
		class abstract_panel;

		class widget {
			
			//	a panel is a widget-container widget
			friend class abstract_panel;

		public:
			//	x,y in parent coordinate system
			widget(const int x, const int y, const unsigned int width, const unsigned int height);
			widget(const rectangle& rect);
			virtual ~widget();

			int get_x(void) const;
			int get_y(void) const;
			unsigned int get_width(void) const;
			unsigned int get_height(void) const;
			const rectangle& get_absolute_rect() const;

			virtual void redraw();

			// x and y in widget coordinate system after this line
			
			virtual bool contains(const int px, const int py) const;
			virtual bool contains(const rectangle& rect) const;
			
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

			virtual void set_pos(const int x, const int y);
			virtual void resize(const unsigned int width, const unsigned int height);
			virtual void set_rect(const rectangle& rect);

		protected:
			void redraw_parent();
			abstract_panel *get_parent();
			const rectangle get_relative_rect() const;	//	self coordinate system
			
		private:
			abstract_panel *m_parent;
			rectangle m_absolute_rect;		//	Parent coordinate system
		};


	} /* View */

}	/* Gammou */

#endif
