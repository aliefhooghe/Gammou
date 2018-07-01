
#include "abstract_win32_display.h"

#define WNDCLASS_NAME "WNDCLASS_NAME"
#define RECT_REDRAW_SHIFT 10

namespace Gammou {

	namespace View {

		abstract_win32_display::abstract_win32_display(View::widget& root_widget)
		:	abstract_display(root_widget),
			m_window_handle(nullptr),
			m_has_focus(false),
			m_is_open(false)
		{
			WNDCLASS window_class;

			window_class.style = CS_DBLCLKS;
			window_class.lpfnWndProc = windowProc;
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

		abstract_win32_display::~abstract_win32_display()
		{
		}

		bool abstract_win32_display::is_open()
		{
			return m_is_open;
		}

		void abstract_win32_display::close()
		{
			if (m_window_handle)
				DestroyWindow(m_window_handle);
			m_window_handle = nullptr;
			m_is_open = false;
		}

		void abstract_win32_display::create_window(
			HWND parent_window, 
			const std::string & title)
		{
			// System coordinate
			const unsigned int system_width = get_display_width();
			const unsigned int system_height = get_display_height();

			if (m_window_handle != nullptr)
				DestroyWindow(m_window_handle);

			m_window_handle =
				CreateWindowA(
					WNDCLASS_NAME, title.c_str(),
					/*WS_CHILD |*/ WS_VISIBLE,
					0, 0, system_width, system_height,
					parent_window, nullptr, nullptr, this);

			// resize
			SetWindowPos(
				m_window_handle, nullptr, 0, 0, system_width, system_height,
				SWP_NOZORDER | SWP_NOMOVE | SWP_NOACTIVATE);

			m_is_open = true;
		}

		void abstract_win32_display::sys_redraw_rect(
			const rectangle & rect)
		{
			DEBUG_PRINT("WIN 32 DISPLAY redraw rect\n");

			if (m_window_handle != nullptr) {
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

				InvalidateRect(m_window_handle, &win32_rect, true);
			}
		}

		HWND abstract_win32_display::get_window_handle() const
		{
			return m_window_handle;
		}

		LRESULT abstract_win32_display::windowProc(HWND window, UINT msg, WPARAM w_param, LPARAM l_param)
		{
			if (msg == WM_CREATE) {
				LPCREATESTRUCT lpcs = (LPCREATESTRUCT)l_param;
				SetWindowLongPtr(window, GWLP_USERDATA, (LPARAM)(lpcs->lpCreateParams));
				return 0;
			}
			else {
				abstract_win32_display *display =
					(abstract_win32_display*)GetWindowLongPtr(window, GWLP_USERDATA);

				switch (msg) {

				case WM_PAINT:
				{
					// System coordinate
					const unsigned int system_width =
						display->get_display_width();
					const unsigned int system_height =
						display->get_display_height();

					PAINTSTRUCT ps;

					HDC screen_dc = BeginPaint(window, &ps);
					HDC mem_dc = CreateCompatibleDC(screen_dc);
					HBITMAP mem_bitmap = CreateCompatibleBitmap(screen_dc, system_width, system_height);

					SelectObject(mem_dc, mem_bitmap);

					cairo_surface_t *cairo_surface =
						cairo_win32_surface_create(mem_dc);
					cairo_t *cr = cairo_create(cairo_surface);

					display->sys_draw(cr);

					cairo_surface_finish(cairo_surface);
					cairo_destroy(cr);
					cairo_surface_destroy(cairo_surface);

					BitBlt(
						screen_dc, 0, 0,
						system_width, system_height,
						mem_dc, 0, 0, SRCCOPY);

					DeleteObject(mem_bitmap);
					DeleteDC(mem_dc);
					DeleteDC(screen_dc);
					EndPaint(window, &ps);
					return 0;
				}
				break;

				case WM_MOUSEMOVE:
					if (!display->m_has_focus) { // Mouse Enter
						tagTRACKMOUSEEVENT track_mouse_event_param;
						track_mouse_event_param.cbSize =
							sizeof(tagTRACKMOUSEEVENT);  // (magnifique)
						track_mouse_event_param.dwFlags = TME_LEAVE; // demande for leave notification 
						track_mouse_event_param.hwndTrack = window;
						TrackMouseEvent(&track_mouse_event_param);
						display->sys_mouse_enter();
						display->m_has_focus = true;
					}

					display->sys_mouse_move(
						GET_X_LPARAM(l_param),
						GET_Y_LPARAM(l_param));

					return 0;
					break;

				case WM_MOUSELEAVE:
					display->sys_mouse_exit();
					display->m_has_focus = false;
					return 0;
					break;

				case WM_LBUTTONDOWN:
					display->sys_mouse_button_down(mouse_button::LeftButton);
					return 0;
					break;

				case WM_RBUTTONDOWN:
					display->sys_mouse_button_down(mouse_button::RightButton);
					return 0;
					break;

				case WM_LBUTTONUP:
					display->sys_mouse_button_up(mouse_button::LeftButton);
					return 0;
					break;

				case WM_RBUTTONUP:
					display->sys_mouse_button_up(mouse_button::RightButton);
					return 0;
					break;

				case WM_LBUTTONDBLCLK:
					display->sys_mouse_dbl_click();
					return 0;
					break;

				default:
					return DefWindowProc(window, msg, w_param, l_param);
					break;
				}
			}
		}


	} /* View */

} /* Gammou */