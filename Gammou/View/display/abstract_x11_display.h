#ifndef ABSTRACT_WIN32_DISPLAY_H_
#define ABSTRACT_WIN32_DISPLAY_H_

#include "cairo_definition.h"
#include "abstract_display.h"

#include <thread>

#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/Xutil.h>
#include <X11/extensions/Xdbe.h>

namespace Gammou {

	namespace View {

		class abstract_x11_display : 
			virtual public abstract_display {

		public:
			abstract_x11_display(View::widget& root_widget);
			virtual  ~abstract_x11_display();

            bool is_open() override;

			virtual void close() override;

		protected:
			void create_window(Display *parent_display, const std::string& title);
			void sys_redraw_rect(const rectangle& rect);
			void wait_window_thread();

			void *get_sys_window_handle() override;
		private:
            static void x_event_loop(abstract_x11_display *self);

            std::thread m_event_loop_thread;

			bool m_running;

            Display *m_display;
            Window m_window;
            Window m_root_window;
            XdbeBackBuffer m_back_buffer; // doubl buffering
            XVisualInfo *m_xvisual_info_found;
            GC m_graphic_context;
            Atom m_wm_delete_message;

            cairo_surface_t *m_cairo_surface;
		};

	} /* View */

} /* Gammou */

#endif