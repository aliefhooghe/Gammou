
#include "win32_application_display.h"

namespace Gammou {

	namespace View {

		win32_application_display::win32_application_display(
			View::widget & root_widget)
		:	abstract_display(root_widget),
			abstract_win32_display(root_widget),
			abstract_application_display(root_widget),
			m_running(false)
		{
		}

		win32_application_display::~win32_application_display()
		{
		}

		void win32_application_display::open(
			const std::string & title)
		{
			create_window((HWND)nullptr, title);
			
			ShowWindow(get_window_handle(), SW_SHOW);
			UpdateWindow(get_window_handle());
			SetFocus(get_window_handle());

			m_running = true;
			m_event_loop = std::thread(win_event_loop, this);
			/*
			win32_application_display *self = this;
			MSG msg;

			DEBUG_PRINT("Entering Win Event Loop\n");

			while (true) {
				DEBUG_PRINT("Getting message.....\n");

				if (GetMessage(&msg, self->get_window_handle(), 0, 0) > 0) {
					DEBUG_PRINT("Got  message\n");
					TranslateMessage(&msg);
					DispatchMessage(&msg);
				}
				else {
					DEBUG_PRINT("GEt message error\n");
				}
			}
			*/
		}

		void win32_application_display::close()
		{
			abstract_win32_display::close();
			if (m_event_loop.joinable())
				m_event_loop.join();
		}

		void win32_application_display::win_event_loop(
			win32_application_display *self)
		{
			MSG msg;

			DEBUG_PRINT("Entering Win Event Loop\n");

			while (true) {
				DEBUG_PRINT("Getting message.....\n");

				if (GetMessage(&msg, self->get_window_handle(), 0, 0) > 0) {
					DEBUG_PRINT("Got  message\n");
					TranslateMessage(&msg);
					DispatchMessage(&msg);
				}
				else {
					DEBUG_PRINT("GEt message error\n");
				}
			}
			
			// TODO destroy win
		}

	}	/* View */

} /* Gammou */