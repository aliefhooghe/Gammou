

#include <cairo-win32.h>

#include "../../../debug.h"
#include "win32_vst3_window.h"

#define WNDCLASS_NAME "WNDCLASS_NAME"
#define RECT_REDRAW_SHIFT 10

namespace Gammou {

	namespace View {

		win32_vst3_window::win32_vst3_window(const unsigned int width, const unsigned int height)
			: abstract_vst3_window(width, height), 
				m_plugin_window(nullptr),
				m_has_focus(false),
				m_cursor_is_visible(true)
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
		}

		win32_vst3_window::~win32_vst3_window()
		{
			close();
			DEBUG_PRINT("Win32 Vst3 Window DTOR\n");
		}

		abstract_vst3_view * win32_vst3_window::create_vst3_view_instance()
		{
			return new win32_vst3_view(this);
		}

		void win32_vst3_window::resize(const unsigned int width, const unsigned int height)
		{
			abstract_vst3_window::resize(width, height);

			if (m_plugin_window != nullptr) {
				const unsigned int system_width = get_system_window_width();
				const unsigned int system_height = get_system_window_height();
				SetWindowPos(m_plugin_window, nullptr, 0, 0, system_width, system_height, SWP_NOZORDER | SWP_NOMOVE | SWP_NOACTIVATE);
			}
		}

		void win32_vst3_window::open(void * parent_window)
		{
			// Widget coordinate
			const unsigned int width = get_width();
			const unsigned int height = get_height();

			// System coordinate
			const unsigned int system_width = get_system_window_width();
			const unsigned int system_height = get_system_window_height();

			HWND parent_hwnd = (HWND)parent_window;
		
			if (m_plugin_window != nullptr) 
				DestroyWindow(m_plugin_window);

			m_plugin_window = CreateWindow(TEXT(WNDCLASS_NAME), TEXT(""/*title*/),
				WS_CHILD | WS_VISIBLE ,
				0, 0, system_width, system_height,
				parent_hwnd, nullptr, nullptr, this);

			resize(width, height);
		}

		void win32_vst3_window::close(void)
		{
			if (m_plugin_window != nullptr)
				DestroyWindow(m_plugin_window);
		}

		void win32_vst3_window::system_redraw_rect(const rectangle & rect)
		{
			rectangle to_draw;

			to_draw.x = (rect.x > RECT_REDRAW_SHIFT) ? (rect.x - RECT_REDRAW_SHIFT) : 0;
			to_draw.y = (rect.y > RECT_REDRAW_SHIFT) ? (rect.y - RECT_REDRAW_SHIFT) : 0;
			to_draw.width = rect.width + RECT_REDRAW_SHIFT * 2;
			to_draw.height = rect.height + RECT_REDRAW_SHIFT * 2;

			RECT win32_rect =
			{
				(LONG)to_draw.x ,
				(LONG)to_draw.y,
				(LONG)to_draw.x + (LONG)to_draw.width,
				(LONG)to_draw.y + (LONG)to_draw.height
			};

			InvalidateRect(m_plugin_window, &win32_rect, true);
		}

		void win32_vst3_window::redraw(void)
		{
			if (m_plugin_window != nullptr)
				InvalidateRect(m_plugin_window, nullptr, true);
		}

		bool win32_vst3_window::open_file(std::string & path, const std::string& title, const std::string & ext)
		{
			OPENFILENAMEA dialog;
			
			const char *cext = ext.c_str();
			const unsigned int ext_len = static_cast<unsigned int>(ext.size());
			char cpath[256] = ""; // ..[0] = '\0'

			char filter[256];
			
			// 
			for (unsigned int i = 0; i < ext_len; ++i) {
				filter[i] = cext[i];
				filter[i + ext_len + 3] = cext[i];
			}

			filter[ext_len] = '\0';
			filter[ext_len + 1] = '*';
			filter[ext_len + 2] = '.';
			filter[(2 * ext_len) + 3] = '\0';
			filter[(2 * ext_len) + 4] = '\0';

			std::memset(&dialog, 0, sizeof(dialog));

			dialog.lStructSize = sizeof(dialog); 
			dialog.hwndOwner = nullptr;
			dialog.hInstance = nullptr; //
			dialog.lpstrFilter = filter;
			dialog.lpstrFile = cpath;
			dialog.nMaxFile = 256;
			dialog.lpstrTitle = title.c_str();
			dialog.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

			if (GetOpenFileNameA(&dialog)) {
				path = std::string(cpath);
				return true;
			}

			return false;
		}
		/*
		void win32_vst3_window::show_cursor(const bool state)
		{
			if (state != m_cursor_is_visible) {
				m_cursor_is_visible = state;
				ShowCursor(state);
			}
		}
		*/
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
						// System coordinate
						const unsigned int system_width = vst3_win->get_system_window_width();
						const unsigned int system_height = vst3_win->get_system_window_height();

						PAINTSTRUCT ps;

						HDC screen_dc = BeginPaint(window, &ps);
						HDC mem_dc = CreateCompatibleDC(screen_dc);
						HBITMAP mem_bitmap = CreateCompatibleBitmap(screen_dc, system_width, system_height);

						SelectObject(mem_dc, mem_bitmap);
						
						cairo_surface_t *cairo_surface = cairo_win32_surface_create(mem_dc);
						cairo_t *cr = cairo_create(cairo_surface);

						vst3_win->sys_draw(cr);		
						cairo_surface_finish(cairo_surface);
						cairo_destroy(cr);
						cairo_surface_destroy(cairo_surface);

						BitBlt(screen_dc, 0, 0, system_width, system_height, mem_dc, 0, 0, SRCCOPY);

						DeleteObject(mem_bitmap);
						DeleteDC(mem_dc);
						DeleteDC(screen_dc);
						EndPaint(window, &ps);
						return 0;
					}
					break;

					case WM_MOUSEMOVE:
						if ( !vst3_win->m_has_focus ) { // Mouse enter
							tagTRACKMOUSEEVENT track_mouse_event_param;
							track_mouse_event_param.cbSize = sizeof(tagTRACKMOUSEEVENT);  // (magnifique)
							track_mouse_event_param.dwFlags = TME_LEAVE; // demande for leave notification 
							track_mouse_event_param.hwndTrack = window;
							TrackMouseEvent(&track_mouse_event_param);
							vst3_win->sys_mouse_enter();
							vst3_win->m_has_focus = true;

							// Make cursor invisible in our windows (according to m_cursos_is_visible)
							//if (!(vst3_win->m_cursor_is_visible)) {
							//	ShowCursor(false);
							//}
						}
						vst3_win->sys_mouse_move(GET_X_LPARAM(l_param), GET_Y_LPARAM(l_param));
						return 0;
						break;

					case WM_MOUSELEAVE:
						vst3_win->sys_mouse_exit();
						vst3_win->m_has_focus = false;

						// Restor cursor for the host
						//if (!(vst3_win->m_cursor_is_visible))
						//	ShowCursor(true);
				
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


		//	win_32_vst3_view implementation

		win32_vst3_view::win32_vst3_view(win32_vst3_window * window)
			: abstract_vst3_view(window)
		{
			DEBUG_PRINT("Win32 Vst3 View CTOR\n");
		}

		win32_vst3_view::~win32_vst3_view()
		{
			DEBUG_PRINT("Win32 Vst3 View View DTOR\n");
		}

		Steinberg::tresult PLUGIN_API win32_vst3_view::isPlatformTypeSupported(Steinberg::FIDString type)
		{
			// win32 Windows == HWND
			return (0 == std::strcmp(Steinberg::kPlatformTypeHWND, type) ?
				Steinberg::kResultTrue : Steinberg::kResultFalse);
		}





} /* View */
} /* Gammou */


