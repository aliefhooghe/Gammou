

#include "win32_vst2_display.h"

namespace Gammou {

	namespace View {

		win32_vst2_display::win32_vst2_display(
			View::widget& root_widget)
			: abstract_display(root_widget),
			abstract_win32_display(root_widget),
			abstract_vst2_display(root_widget)
		{
		}

		win32_vst2_display::~win32_vst2_display()
		{
		}

		void win32_vst2_display::open(void *host_parent_window)
		{
			m_is_open = true;
			create_window((HWND)host_parent_window, "");
		}

		void win32_vst2_display::close()
		{
			m_is_open = false;
			destroy_window();
		}
	}
}