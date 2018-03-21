
#include "x11_app_window.h"

namespace Gammou {

    namespace View {

        x11_app_window::x11_app_window(const unsigned int px_width, const unsigned int px_height)
            : abstract_app_window(px_width, px_height)
        {
            int screen;
            Drawable dra;

            m_display = XOpenDisplay(nullptr);

            if( m_display == nullptr )
                throw std::runtime_error("Unable to open X Display");

            screen = DefaultScreen(m_display);
            dra = XCreateSimpleWindow(
                m_display, DefaultRootWindow(m_display), 
                0, 0, px_width, px_height, 0, 0, 0);
            
            XSelectInput(m_display, dra, 
                KeyPressMask | KeyReleaseMask | ExposureMask |
                PointerMotionMask | ButtonPressMask | ButtonReleaseMask);

            XMapWindow(m_display, dra);

            m_cairo_surface = cairo_xlib_surface_create(
                m_display, dra, 
                DefaultVisual(m_display, screen),
                px_width, px_height
            );

            cairo_xlib_surface_set_size(
                m_cairo_surface,
                px_width, px_height
            );

            m_event_loop_thread = std::thread(x_event_loop, this);
        }

        x11_app_window::~x11_app_window()
        {
            cairo_surface_destroy(m_cairo_surface);
            XCloseDisplay(m_display);
        }

        void x11_app_window::open()
        {
            // todo
        }

        void x11_app_window::close()
        {
            // todo
        }

        bool x11_app_window::open_file(std::string& path, const std::string& title, const std::string& ext)
        {
            return false;
        }
        
        void x11_app_window::system_redraw_rect(const rectangle& rect)
        {
            // todo
        }

        // 
        void x11_app_window::redraw(void)
        {
            // todo
        }
        
        void x11_app_window::resize(const unsigned int width, const unsigned int height)
        {
            // Todo
        }


        void x11_app_window::x_event_loop(x11_app_window *self)
        {
            cairo_t *cr = cairo_create(self->m_cairo_surface);

            for(;;){
                XEvent event;

                XNextEvent(self->m_display, &event);

                switch( event.type ){

                    case ButtonPress:
                        DEBUG_PRINT("ButtonPress\n");
                        break;

                    case ButtonRelease:
                        break;

                    case KeyPress:
                        break;

                    case KeyRelease:
                        break;

                    case MotionNotify:
                        break;

                    case Expose:
                        DEBUG_PRINT("Expose\n");
                        
                        
                       // cairo_rectangle(cr, 100, 100, 300, 300);

                       // cairo_set_source_rgb(cr, 1.0, 0.0, 0.0);
                       // cairo_fill_preserve(cr);
                        //cairo_set_source_rgb(cr, 0.0, 1.0, 0.0);
                       // cairo_stroke(cr);

                        self->sys_draw(cr);
                        
                        cairo_surface_flush(self->m_cairo_surface);
                        //XFlush(self->m_display);
                        //cairo_destroy(cr);
                        break;
                }
            }

        }
    }

}