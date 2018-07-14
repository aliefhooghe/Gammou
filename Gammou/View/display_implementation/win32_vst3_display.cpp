
#include "win32_vst3_display.h"

namespace Gammou {

	namespace View {

		// win32_vst3 display implementation

		win32_vst3_display::win32_vst3_display(View::widget& root_widget)
		:	abstract_display(root_widget),
			abstract_win32_display(root_widget),
			abstract_vst3_display(root_widget)
		{
			DEBUG_PRINT("WIN32 VST3 DISPALY CTOR\n");
		}

		win32_vst3_display::~win32_vst3_display()
		{
		}

		void win32_vst3_display::open(void * host_parent_window)
		{
			DEBUG_PRINT("WIN32 VST3 DISPLAY Open Windows\n");
			m_is_open = true;
			create_window((HWND)host_parent_window, "");
		}

		void win32_vst3_display::close()
		{
			m_is_open = false;
			destroy_window();
		}

		abstract_vst3_view * win32_vst3_display::create_vst3_view_instance()
		{
			return new win32_vst3_view(this);
		}

		//	win_32_vst3_view implementation

		win32_vst3_view::win32_vst3_view(win32_vst3_display * display)
			: abstract_vst3_view(display)
		{
			DEBUG_PRINT("Win32 Vst3 View CTOR\n");
		}

		win32_vst3_view::~win32_vst3_view()
		{
			DEBUG_PRINT("Win32 Vst3 View View DTOR\n");
		}

		Steinberg::tresult PLUGIN_API win32_vst3_view::isPlatformTypeSupported(
			Steinberg::FIDString type)
		{
			// win32 Windows == HWND
			return (0 == std::strcmp(Steinberg::kPlatformTypeHWND, type) ?
				Steinberg::kResultTrue : Steinberg::kResultFalse);
		}

	} /* View */

} /* Gammou */