#include "debug.h"
#include "gammou_win32_vst3_window.h"

#define WNDCLASS_NAME "WNDCLASS_NAME"

namespace Gammou {

	namespace View {

		win32_vst3_window::win32_vst3_window(const unsigned int width, const unsigned int height)
			: abstract_vst3_window(width, height), m_has_focus(false)
		{
			WNDCLASS window_class;

			window_class.style = CS_DBLCLKS;
			window_class.lpfnWndProc = &windowProc;
			window_class.cbClsExtra = 0;
			window_class.cbWndExtra = 0;
			window_class.hInstance = nullptr;
			window_class.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
			window_class.hCursor = LoadCursor(nullptr, IDC_ARROW);
			window_class.hbrBackground = nullptr;
			window_class.lpszMenuName = nullptr;
			window_class.lpszClassName = TEXT(WNDCLASS_NAME);

			RegisterClass(&window_class);

			m_plugin_window = nullptr;
		}

		win32_vst3_window::~win32_vst3_window()
		{
			if( m_plugin_window != nullptr )
				DestroyWindow(m_plugin_window);
		}

		abstract_vst3_view * win32_vst3_window::create_vst3_view_instance()
		{
			return new win32_vst3_view(this);
		}

		void win32_vst3_window::resize(const unsigned int width, const unsigned int height)
		{
			abstract_vst3_window::resize(width, height);
			if (m_plugin_window != nullptr) 
				SetWindowPos(m_plugin_window, nullptr, 0, 0, width, height, SWP_NOZORDER | SWP_NOMOVE | SWP_NOACTIVATE);
		}

		void win32_vst3_window::open(void * parent_window)
		{
			const unsigned int width = get_width();
			const unsigned int height = get_height();
			HWND parent_hwnd = (HWND)parent_window;
		
			if (m_plugin_window != nullptr) 
				DestroyWindow(m_plugin_window);

			m_plugin_window = CreateWindow(TEXT(WNDCLASS_NAME), TEXT(""/*titre*/),
				WS_CHILD | WS_VISIBLE ,
				0, 0, width, height,
				parent_hwnd, nullptr, nullptr, this);

			resize(width, height);
		}

		void win32_vst3_window::close(void)
		{
			if (m_plugin_window != nullptr)
				DestroyWindow(m_plugin_window);
		}

		void win32_vst3_window::redraw_rect(rectangle & rect)
		{
			RECT win32_rect = 
			{ rect.x , rect.y, rect.x + rect.width, rect.y + rect.height };
			InvalidateRect(m_plugin_window, &win32_rect, true);
		}


		void win32_vst3_window::redraw(void)
		{
			if (m_plugin_window != nullptr)
				InvalidateRect(m_plugin_window, nullptr, true);
		}

		LRESULT win32_vst3_window::windowProc(HWND window, UINT msg, WPARAM w_param, LPARAM l_param)
		{
			if (msg == WM_CREATE) {
				LPCREATESTRUCT lpcs = (LPCREATESTRUCT)l_param;
				SetWindowLongPtr(window, GWLP_USERDATA, (LPARAM)(lpcs->lpCreateParams));
				return 0;
			}
			else {
				win32_vst3_window *vst3_win =
					(win32_vst3_window*)GetWindowLongPtr(window, GWLP_USERDATA);

				switch (msg) {

					case WM_PAINT:
					{
						PAINTSTRUCT ps;
						HDC screen_dc = BeginPaint(window, &ps);
						HDC mem_dc = CreateCompatibleDC(screen_dc);
						HBITMAP mem_bitmap = CreateCompatibleBitmap(screen_dc, vst3_win->get_width(), vst3_win->get_height());
						SelectObject(mem_dc, mem_bitmap);
						
						cairo_surface_t *cairo_surface = cairo_win32_surface_create(mem_dc);
						cairo_t *cr = cairo_create(cairo_surface);
						vst3_win->sys_draw(cr);		
						cairo_surface_finish(cairo_surface);
						cairo_destroy(cr);
						cairo_surface_destroy(cairo_surface);

						BitBlt(screen_dc, 0, 0, vst3_win->get_width(), vst3_win->get_height(), mem_dc, 0, 0, SRCCOPY);

						DeleteObject(mem_bitmap);
						DeleteDC(mem_dc);
						DeleteDC(screen_dc);
						EndPaint(window, &ps);
						return 0;
					}
					break;

					case WM_MOUSEMOVE:
						if ( !vst3_win->m_has_focus ) {
							tagTRACKMOUSEEVENT track_mouse_event_param;
							track_mouse_event_param.cbSize = sizeof(tagTRACKMOUSEEVENT);  // (magnifique)
							track_mouse_event_param.dwFlags = TME_LEAVE; // demande for leave notification 
							track_mouse_event_param.hwndTrack = window;
							TrackMouseEvent(&track_mouse_event_param);
							vst3_win->sys_mouse_enter();
							DEBUG_PRINT("ENTER\n");
							vst3_win->m_has_focus = true;
						}
						vst3_win->sys_mouse_move(GET_X_LPARAM(l_param), GET_Y_LPARAM(l_param));
						return 0;
						break;

					case WM_MOUSELEAVE:
						DEBUG_PRINT("EXIT\n");
						vst3_win->sys_mouse_exit();
						vst3_win->m_has_focus = false;
						return 0;
						break;

					case WM_LBUTTONDOWN:
						vst3_win->sys_mouse_button_down(mouse_button::LeftButton);
						return 0;
						break;

					case WM_RBUTTONDOWN:
						vst3_win->sys_mouse_button_down(mouse_button::RightButton);
						return 0;
						break;

					case WM_LBUTTONUP:
						vst3_win->sys_mouse_button_up(mouse_button::LeftButton);
						return 0;
						break;

					case WM_RBUTTONUP:
						vst3_win->sys_mouse_button_up(mouse_button::RightButton);
						return 0;
						break;

					case WM_LBUTTONDBLCLK:
						vst3_win->sys_mouse_dbl_click();
						return 0;
						break;

					default:
						return DefWindowProc(window, msg, w_param, l_param);
						break;
					}
			}
			

		}


		//	win_32_vst3_viw implementation
		//	todo peut etre plus de code dans abstract_vst3_view

		win32_vst3_view::win32_vst3_view(win32_vst3_window * window)
			: abstract_vst3_view(window)
		{
			DEBUG_PRINT("View CTOR\n");
		}

		win32_vst3_view::~win32_vst3_view()
		{
			DEBUG_PRINT("View DTOR\n");
		}

		Steinberg::tresult PLUGIN_API win32_vst3_view::isPlatformTypeSupported(Steinberg::FIDString type)
		{
			return (0 == std::strcmp(Steinberg::kPlatformTypeHWND, type) ?
				Steinberg::kResultTrue : Steinberg::kResultFalse);
		}

		void win32_vst3_view::attachedToParent()
		{
			if (m_window != nullptr)
				m_window->open(systemWindow);
		}

		void win32_vst3_view::removedFromParent()
		{
			if (m_window != nullptr)
				m_window->close();
		}



} /* View */
} /* Gammou */

