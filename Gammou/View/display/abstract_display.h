#ifndef ABSTRACT_DISPLAY_H_
#define ABSTRACT_DISPLAY_H_

#include <memory>
#include "widget/panel.h"

namespace Gammou {

	namespace View {
		
		class abstract_display : private abstract_panel {

		public:
			abstract_display(widget& root_widget);
			virtual ~abstract_display();

			double get_scale_factor() const;
			virtual void set_scale_factor(const float scale_factor);

			unsigned int get_display_width() const;
			unsigned int get_display_height() const;

			virtual bool is_open() = 0;
			virtual void close() = 0;
			virtual void non_blocking_close() = 0;

		protected:
			//	'Low level' callback, called by system event processing, 
			//	and translated into 'higher level' events.
			//	In window system coordinate (display)
			void sys_draw(cairo_t *cr);
			bool sys_mouse_move(const unsigned int cx, const unsigned int cy);
			bool sys_mouse_enter(void);
			bool sys_mouse_exit(void);
			bool sys_mouse_button_down(const mouse_button button);
			bool sys_mouse_button_up(const mouse_button button);
			bool sys_mouse_wheel(const float distance);
			bool sys_mouse_dbl_click(void);
			bool sys_key_down(const keycode key);
			bool sys_key_up(const keycode key);

			virtual void sys_redraw_rect(const rectangle& rect) =0;

		private:
			abstract_display *get_display() override;
			void redraw_rect(const rectangle& rect) override;
			void draw(cairo_t *cr) override;

			widget& m_root_widget;

			float m_scale_factor;

			unsigned int m_display_width;
			unsigned int m_display_height;

			unsigned int m_cursor_x;
			unsigned int m_cursor_y;

			bool m_is_draging;
			mouse_button m_draging_button;
			unsigned int m_pressed_mouse_button_count;
		};

	} /* View */

} /* Gammou */

#endif