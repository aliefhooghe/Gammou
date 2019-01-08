
#include <cstring>
#include <sys/select.h>

#include "abstract_x11_display.h"

#define GAMMOU_X_EVENT_MASK KeyPressMask | KeyReleaseMask | ExposureMask | StructureNotifyMask | \
                            LeaveWindowMask | PointerMotionMask | ButtonPressMask | ButtonReleaseMask | \
                            EnterWindowMask | ConfigureNotify

namespace Gammou {

	namespace View {

        abstract_x11_display::abstract_x11_display(View::widget& root_widget)
        :   abstract_display(root_widget),
            m_running(false)
        {
            static bool x_thread_init = false;

            if (!x_thread_init) {
                XInitThreads();
                x_thread_init = true;
            }
        }

        abstract_x11_display::~abstract_x11_display()
        {
            DEBUG_PRINT("abstract_x11_display DTOR\n");
            close();
        }

        bool abstract_x11_display::is_open()
        {
            return m_running;
        }

        void abstract_x11_display::close()
        {
            DEBUG_PRINT("abstract_x11_display Close\n");

            m_running = false;

            if (std::this_thread::get_id() != 
                    m_event_loop_thread.get_id())
                wait_window_thread();
        }

        void abstract_x11_display::create_window(
            Window parent, 
            const std::string& title)
        {
            DEBUG_PRINT("Abstract X11 Display Opening...\n");

            if (m_running)
                return;

            const unsigned int px_width = get_display_width();
            const unsigned int px_height = get_display_height();

            m_display = XOpenDisplay(nullptr);

            if( m_display == nullptr )
                throw std::runtime_error("Unable to open X Display");
            
            // Check we can use Xbde
            int minor_version = 0, major_version = 0;
            if(!XdbeQueryExtension(m_display, &major_version, &minor_version))
                throw std::runtime_error("Xbde unsuported\n");

            int screen_count = 1;
            m_root_window = DefaultRootWindow(m_display);

            XdbeScreenVisualInfo *info = 
                XdbeGetVisualInfo(m_display, &m_root_window, &screen_count);

            if( info == nullptr || screen_count < 0 || info->count < 1)
                throw std::runtime_error("Xbde unsuported\n");
            
            XVisualInfo xvisual_info_template;

            xvisual_info_template.visualid = info->visinfo[0].visual;
            xvisual_info_template.screen = 0;
            xvisual_info_template.depth = info->visinfo[0].depth;

            int found_count = 0;
                
            m_xvisual_info_found
                = XGetVisualInfo(m_display,
                VisualIDMask | VisualScreenMask | VisualDepthMask, 
                &xvisual_info_template, &found_count);
            
            if( m_xvisual_info_found == nullptr || found_count < 1 )
                throw std::runtime_error("No Visual With Double Buffering\n");

            const int screen = DefaultScreen(m_display);

            XSetWindowAttributes xattributs;
            bzero(&xattributs, sizeof(XSetWindowAttributes));
            xattributs.background_pixel = WhitePixel(m_display, screen);

            m_window = XCreateWindow(
                m_display, m_root_window,   //  <-
                0, 0, px_width, px_height, 0, 
                CopyFromParent, CopyFromParent, 
                m_xvisual_info_found->visual, 
                CWBackPixel, &xattributs);

            if (parent != 0)
                XReparentWindow(m_display, m_window, parent, 0, 0);
            
            // Enbale window close event
            m_wm_delete_message = XInternAtom(m_display, "WM_DELETE_WINDOW", false);
            XSetWMProtocols(m_display, m_window, &m_wm_delete_message, 1);

            XSizeHints constrain;
            std::memset(&constrain, 0, sizeof(constrain));

            constrain.flags = PMinSize | PMaxSize;
            constrain.max_width = static_cast<int>(px_width);
            constrain.min_width = static_cast<int>(px_width);
            constrain.max_height = static_cast<int>(px_height);
            constrain.min_height = static_cast<int>(px_height);

            XSetWMNormalHints(m_display, m_window, &constrain);

            // 
            m_back_buffer = XdbeAllocateBackBufferName(m_display, m_window, XdbeBackground);
            
            //--
            XSelectInput(m_display, m_window, GAMMOU_X_EVENT_MASK);

            //  Map The Window
            XMapWindow(m_display, m_window);

            // to be sure that windows is mapped
            DEBUG_PRINT("Waiting map notify event...\n");
            for(XEvent e; e.type != MapNotify; XNextEvent(m_display, &e));
            DEBUG_PRINT("Map Notify event received\n");

            //  Set Window Name
            XStoreName(m_display, m_window, title.c_str());

            //  Create Graphic Context
            m_graphic_context = XCreateGC(m_display, m_back_buffer, 0, nullptr);
            XSetForeground(m_display, m_graphic_context, BlackPixel(m_display, screen));

            // Cairo
            m_cairo_surface = cairo_xlib_surface_create(
                m_display, m_back_buffer, 
                m_xvisual_info_found->visual,
                px_width, px_height);

            cairo_xlib_surface_set_size(
                m_cairo_surface,
                px_width, px_height);

            m_cr = cairo_create(m_cairo_surface);

            DEBUG_PRINT("Gui Thread Start\n");

            m_running = true;
            m_event_loop_thread = std::thread(x_event_loop, this);
        }

        void abstract_x11_display::sys_redraw_rect(
            const rectangle& rect)
        {
            if (m_display == nullptr)
                return;
                
            XEvent ev;
            std::memset(&ev, 0, sizeof(ev));

            ev.type = Expose;

            ev.xexpose.x = rect.x;
            ev.xexpose.y = rect.y;
            ev.xexpose.width = rect.width;
            ev.xexpose.height = rect.height;
            ev.xexpose.window = m_back_buffer;

            XSendEvent(m_display, m_window, true, ExposureMask, &ev);
        }

        void abstract_x11_display::draw_display()
        {
            sys_draw(m_cr);
            
            // Swap Buffer
            XdbeSwapInfo info;
            info.swap_window = m_window;
            info.swap_action = XdbeBackground;
            XdbeSwapBuffers(m_display, &info, 1);
            XFlush(m_display);
        }

        void abstract_x11_display::wait_window_thread()
        {
             DEBUG_PRINT("abstract_x11_display Wait Window THread\n");

            if (m_event_loop_thread.joinable())
                m_event_loop_thread.join();
        }

        void abstract_x11_display::handle_event(
            abstract_x11_display* self,
            XEvent& event)
        {

            switch( event.type ){

                    case ButtonPress:
                        switch(event.xbutton.button){

                            case 1: // left
                            self->sys_mouse_button_down(
                                mouse_button::LeftButton);
                            break;

                            case 2: // wheel
                            self->sys_mouse_button_down(
                                mouse_button::WheelButton);
                            break;

                            case 3: // right
                            self->sys_mouse_button_down(
                                mouse_button::RightButton);
                            break;

                            case 4:
                            self->sys_mouse_wheel(1.0f);
                            break;

                            case 5:
                            self->sys_mouse_wheel(-1.0f);
                            break;
                        }
                        break;

                    case ButtonRelease:
                        {   
                            const unsigned int button = event.xbutton.button;

                            // Double Click Detection

                            if( button == 1 || button == 3 ){
                                const Time now = event.xbutton.time;

                                if (event.xbutton.button == self->m_last_button) {
                                    const Time delta = 
                                        now - self->m_last_click_time;
                                    
                                    //  
                                    if( delta > 50 && delta < 250 )
                                        self->sys_mouse_dbl_click();
                                }
                                
                                self->m_last_click_time = now;
                                self->m_last_button = event.xbutton.button;
                            }

                            switch(button){
                                case 1: // left
                                self->sys_mouse_button_up(
                                    mouse_button::LeftButton);
                                break;

                                case 2: // wheel
                                self->sys_mouse_button_up(
                                    mouse_button::WheelButton);
                                break;

                                case 3: // right
                                self->sys_mouse_button_up(
                                    mouse_button::RightButton);
                                break;
                            }
                        }
                        break;

                    case KeyPress:
                        self->sys_key_down(
                            convert_key(
                                XLookupKeysym(&(event.xkey), 0)));
                        break;

                    case KeyRelease:
                        self->sys_key_up(
                            convert_key(
                                XLookupKeysym(&(event.xkey), 0)));
                        break;

                    case MotionNotify:
                    {
                        //  Decrease cpu usage by decreasing mouse_move event rate

                        const int interval = 4;
                        
                        if (self->m_motion_notify_count % interval == 0) {
                          self->sys_mouse_move(
                            static_cast<unsigned int>(event.xmotion.x),
                            static_cast<unsigned int>(event.xmotion.y)
                          );
                        }

                        self->m_motion_notify_count++;
                    }
                        break;

                    case Expose:
                        self->m_display_need_redraw = true;
                        break;

                    case EnterNotify:
                        self->sys_mouse_enter();
                        break;
                    
                    case LeaveNotify:
                        self->sys_mouse_exit();
                        break;

                    case ClientMessage:
                        if (event.xclient.data.l[0] == self->m_wm_delete_message) {
                            DEBUG_PRINT("Received window delete xclient messge\n"); 
                            self->m_running = false;
                            
                        }
                        break;
/*
                    case ConfigureNotify:
                        {
                            const auto w = static_cast<int>(self->get_display_width());
                            const auto h = static_cast<int>(self->get_display_height());

                            const auto& conf = event.xconfigure;

                            if  (conf.width != w || conf.height != h) {
                                cairo_xlib_surface_set_size(
                                    self->m_cairo_surface,
                                    conf.width, conf.height);
                            }
                        }
                        break;
*/

                    default:
                        break;

                }
        }

#define X_EVENT_LOOP_TIMEOUT_USEC 100000

        void abstract_x11_display::x_event_loop(
            abstract_x11_display *self)
        {
            int xevent_queue_fd = ConnectionNumber(self->m_display);
            struct timeval timeout = {
                .tv_sec = 0,
                .tv_usec = X_EVENT_LOOP_TIMEOUT_USEC
            };

            //  MAke sure that the inwods will be drawn at startup
            self->draw_display();

            while(self->m_running){
                fd_set xevent_queue_set;
                FD_ZERO(&xevent_queue_set);
                FD_SET(xevent_queue_fd, &xevent_queue_set);

                const int ret =
                    select(xevent_queue_fd + 1, &xevent_queue_set, 0, 0, &timeout);

                if (ret < 0) {
                    DEBUG_PRINT("Fatal select error\n");
                    exit(0);
                }
                else if (ret == 0) {
                    //  Timeout was reached (Allow m_running to be checked)
                    timeout.tv_usec = X_EVENT_LOOP_TIMEOUT_USEC;                        
                }
                else {  //  there are event(s) to be read

                    //  While the Event Queue is not empty
                    while (XPending(self->m_display) > 0) {
                        XEvent event;
                        XNextEvent(self->m_display, &event);
                        handle_event(self, event);
                    }

                    if (self->m_display_need_redraw)
                        self->draw_display();
                }
            }

            cairo_destroy(self->m_cr);
            cairo_surface_destroy(self->m_cairo_surface);
            XFreeGC(self->m_display, self->m_graphic_context);
            XDestroyWindow(self->m_display, self->m_window);
            XCloseDisplay(self->m_display);
            self->m_display = nullptr;
            DEBUG_PRINT("Quit thread Xloop function\n");
        }


        #define TRANSLATE_KEY(xk, k) \
            case xk: \
                return k; \
                break;

        keycode abstract_x11_display::convert_key(const KeySym xkey)
        {

            if (xkey >= XK_a && xkey <= XK_z) {
                const unsigned int offset = xkey - XK_a;
                return 
                    static_cast<keycode>(
                        static_cast<unsigned int>(key_A) + 
                        offset);
            }
            else{

                switch (xkey) {
                    TRANSLATE_KEY(XK_BackSpace, key_backspace)
                    TRANSLATE_KEY(XK_Tab, key_tab)
                    TRANSLATE_KEY(XK_Return, key_enter)
                    TRANSLATE_KEY(XK_Shift_L, key_shift)
                    TRANSLATE_KEY(XK_Shift_R, key_shift)
                    TRANSLATE_KEY(XK_Control_L,key_ctrl)
                    TRANSLATE_KEY(XK_Control_R,key_ctrl)
                    TRANSLATE_KEY(XK_Alt_L,key_alt)
                    TRANSLATE_KEY(XK_Alt_R,key_alt)
                    TRANSLATE_KEY(XK_Pause, key_pause_break)
                    TRANSLATE_KEY(XK_Caps_Lock, key_caps_lock)
                    TRANSLATE_KEY(XK_Escape, key_escape)
                    TRANSLATE_KEY(XK_space, key_space)
                    TRANSLATE_KEY(XK_Page_Up, key_page_up)
                    TRANSLATE_KEY(XK_Page_Down, key_page_down)
                    TRANSLATE_KEY(XK_End, key_end)
                    TRANSLATE_KEY(XK_Home, key_home)
                    TRANSLATE_KEY(XK_Left, key_left_arrow)
                    TRANSLATE_KEY(XK_Up, key_up_arrow)
                    TRANSLATE_KEY(XK_Right, key_right_arrow)
                    TRANSLATE_KEY(XK_Down, key_down_arrow)
                    TRANSLATE_KEY(XK_Insert, key_insert)
                    TRANSLATE_KEY(XK_Delete, key_delete)
                    TRANSLATE_KEY(XK_0, key_0)
                    TRANSLATE_KEY(XK_1, key_1)
                    TRANSLATE_KEY(XK_2, key_2)
                    TRANSLATE_KEY(XK_3, key_3)
                    TRANSLATE_KEY(XK_4, key_4)
                    TRANSLATE_KEY(XK_5, key_5)
                    TRANSLATE_KEY(XK_6, key_6)
                    TRANSLATE_KEY(XK_7, key_7)
                    TRANSLATE_KEY(XK_8, key_8)
                    TRANSLATE_KEY(XK_9, key_9)
                    TRANSLATE_KEY(XK_Select, key_select)
                    TRANSLATE_KEY(XK_KP_0, key_numpad_0)
                    TRANSLATE_KEY(XK_KP_1, key_numpad_1)
                    TRANSLATE_KEY(XK_KP_2, key_numpad_2)
                    TRANSLATE_KEY(XK_KP_3, key_numpad_3)
                    TRANSLATE_KEY(XK_KP_4, key_numpad_4)
                    TRANSLATE_KEY(XK_KP_5, key_numpad_5)
                    TRANSLATE_KEY(XK_KP_6, key_numpad_6)
                    TRANSLATE_KEY(XK_KP_7, key_numpad_7)
                    TRANSLATE_KEY(XK_KP_8, key_numpad_8)
                    TRANSLATE_KEY(XK_KP_9, key_numpad_9)
                    TRANSLATE_KEY(XK_KP_Multiply, key_multiply)
                    TRANSLATE_KEY(XK_KP_Add, key_add)
                    TRANSLATE_KEY(XK_KP_Subtract, key_subtract)
                    TRANSLATE_KEY(XK_KP_Decimal, key_decimal_point)
                    TRANSLATE_KEY(XK_KP_Divide, key_divide)
                    TRANSLATE_KEY(XK_F1, key_f1)
                    TRANSLATE_KEY(XK_F2, key_f2)
                    TRANSLATE_KEY(XK_F3, key_f3)
                    TRANSLATE_KEY(XK_F4, key_f4)
                    TRANSLATE_KEY(XK_F5, key_f5)
                    TRANSLATE_KEY(XK_F6, key_f6)
                    TRANSLATE_KEY(XK_F7, key_f7)
                    TRANSLATE_KEY(XK_F8, key_f8)
                    TRANSLATE_KEY(XK_F9, key_f9)
                    TRANSLATE_KEY(XK_F10, key_f10)
                    TRANSLATE_KEY(XK_F11, key_f11)
                    TRANSLATE_KEY(XK_F12, key_f12)
                    TRANSLATE_KEY(XK_Num_Lock, key_num_lock)
                    TRANSLATE_KEY(XK_Scroll_Lock, key_scroll_lock)
                    TRANSLATE_KEY(XK_semicolon, key_semi_colon)
                    TRANSLATE_KEY(XK_equal, key_equal_sign)

                    default:
                        return key_unknown;
                }
            }
        }

    } /* View */
} /* Gammou */


