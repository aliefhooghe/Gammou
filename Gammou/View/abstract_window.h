#ifndef GAMMOU_ABSTRACT_WINDOW_H_
#define GAMMOU_ABSTRACT_WINDOW_H_

#include "widget/panel.h"

namespace Gammou {

	namespace View {

		class abstract_window : public panel<widget> {
		
		public:
			//	Width and Height in pixel : abstract size, real size depend on scale
			abstract_window(const unsigned int px_width, const unsigned int px_height);
			virtual ~abstract_window();

		protected:
			//	'Low level' callback, called by system event processing, 
			//	and translated into 'higher level' events.
			//	/!\ system musn't call directly event handlers !!
			//	In window system coordinate
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

			unsigned int get_system_window_width() const;
			unsigned int get_system_window_height() const;

			// Open and close here ?

			//	Feature to be implemented
			virtual bool open_file(std::string& path, const std::string& title, const std::string& ext) =0;
			//virtual void show_cursor(const bool state = true) = 0;

			//---
			void scale(const float scaling_factor);
			
			void redraw_rect(const rectangle& rect) override;
			virtual void system_redraw_rect(const rectangle& rect) =0;

		private:
			float m_scale_factor;
			
			unsigned int m_cursor_x;
			unsigned int m_cursor_y;

			bool m_is_draging;
			mouse_button m_draging_button;
			unsigned int m_pressed_mouse_button_count;
		};

	} /* View */

} /* namespace Gammou */

#endif