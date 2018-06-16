#ifndef GAMMOU_X11_APP_WINDOW_H_
#define GAMMOU_X11_APP_WINDOW_H_


#include <thread>
#include <cairo-xlib.h>
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/Xutil.h>
#include <X11/extensions/Xdbe.h>


#include "../abstract_app_window.h"

namespace Gammou {

    namespace View {

        class x11_app_window : public abstract_app_window {

            public:
                x11_app_window(const unsigned int px_width, const unsigned int px_height);
                virtual ~x11_app_window();

                void open() override;
                void close() override;

            protected:
                // Abstract windows
                bool open_file(std::string& path, const std::string& title, const std::string& ext) override;
                void system_redraw_rect(const rectangle& rect) override;

                // 
                void redraw(void) override;
                void resize(const unsigned int width, const unsigned int height) override;

            private:
                static void x_event_loop(x11_app_window *self);

                std::thread m_event_loop_thread;
                bool m_running;
                Display *m_display;
                Window m_window;
                Window m_root_window;
                XdbeBackBuffer m_back_buffer; // doubl buffering
                XVisualInfo *m_xvisual_info_found;

                cairo_surface_t *m_cairo_surface;

        };

    }

}


#endif
