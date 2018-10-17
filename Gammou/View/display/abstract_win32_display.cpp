
#include "abstract_win32_display.h"

#define WNDCLASS_NAME "WNDCLASS_NAME"
#define RECT_REDRAW_SHIFT 10

namespace Gammou {

	namespace View {

		abstract_win32_display::abstract_win32_display(View::widget& root_widget)
			: abstract_display(root_widget),
			m_is_open(false),
			m_window_handle(nullptr),
			m_has_focus(false)
		{
		}

		abstract_win32_display::abstract_win32_display(
			abstract_display& parent,
			View::widget& node_widget)
			: abstract_display(node_widget),
			m_is_open(false),
			m_window_handle(nullptr),
			m_has_focus(false)
		{

		}

		abstract_win32_display::~abstract_win32_display()
		{
		}

		bool abstract_win32_display::is_open()
		{
			return m_is_open;
		}

		void abstract_win32_display::create_window(
			HWND parent_window,
			const std::string & title)
		{
			const unsigned int width = get_display_width();
			const unsigned int height = get_display_height();

			register_window_class();	//	make sure this is done in the window thread

			if (m_window_handle != nullptr)
				DestroyWindow(m_window_handle);

			DWORD style = WS_VISIBLE;

			if (parent_window == nullptr)
				style |= (WS_SYSMENU | WS_MINIMIZEBOX);		//	app
			else
				style |= WS_CHILD;							//	vst3

			// Default value, ok for a plugin

			unsigned int real_width = width;
			unsigned int real_height = height;
			int x = 0, y = 0;
			
			if (parent_window == nullptr) {	//	Root window
				//	Make window appear near cursor and 
				//	take care of menu bar height

				POINT p;
				GetCursorPos(&p);

				x = p.x - width / 2;
				y = p.y - height / 2;

				if (x < 0)
					x = 0;
				if (y < 0)
					y = 0;

				real_width += GetSystemMetrics(SM_CYBORDER);
				real_height += 
					(GetSystemMetrics(SM_CYFRAME) + 
						GetSystemMetrics(SM_CYCAPTION));
			}
			
			//

			m_window_handle =
				CreateWindowA(
					WNDCLASS_NAME,
					title.c_str(), style,
					x, y, real_width, real_height,
					parent_window, nullptr, nullptr,
					this);
		}

		void abstract_win32_display::destroy_window()
		{
			if (m_window_handle)
				DestroyWindow(m_window_handle);
			m_window_handle = nullptr;
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

		HWND abstract_win32_display::get_window_handle()
		{
			return m_window_handle;
		}

		void abstract_win32_display::register_window_class()
		{
			static bool have_been_registered = false;

			if (!have_been_registered) {
				WNDCLASS window_class;

				memset(&window_class, 0, sizeof(WNDCLASS));

				window_class.style = CS_DBLCLKS; // CS_HREDRAW |CS_VREDRAW; redraw if size change
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
				have_been_registered = true;
			}
		}

		LRESULT abstract_win32_display::windowProc(HWND window, UINT msg, WPARAM w_param, LPARAM l_param)
		{

			abstract_win32_display *display =
				(abstract_win32_display*)GetWindowLongPtr(window, GWLP_USERDATA);

			switch (msg) {

			case WM_CREATE:
			{
				DEBUG_PRINT("WM Create Received !!\n");
				LPCREATESTRUCT lpcs = (LPCREATESTRUCT)l_param;
				SetWindowLongPtr(window, GWLP_USERDATA, (LPARAM)(lpcs->lpCreateParams));
				return 0;
			}
			break;

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

			case WM_MOUSEWHEEL:
			{
				const auto delta = GET_WHEEL_DELTA_WPARAM(w_param);
				if (delta < 0)
					display->sys_mouse_wheel(-1.0);
				else
					display->sys_mouse_wheel(1.0);
				return 0;
			}
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
			{
				return DefWindowProc(window, msg, w_param, l_param);
			}
				break;
			}

		}


	} /* View */

} /* Gammou */