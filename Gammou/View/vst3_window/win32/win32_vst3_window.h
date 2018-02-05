#ifndef GAMMOU_WIN32_VST3_WINDOW_H_
#define GAMMOU_WIN32_VST3_WINDOW_H_

#ifdef _WIN32

#include "../abstract_vst3_window.h"
#include <Windows.h>
#include <windowsx.h>

namespace Gammou {

	namespace View {

		class win32_vst3_window : public abstract_vst3_window {
		public:
			win32_vst3_window(const unsigned int width, const unsigned int height);
			virtual ~win32_vst3_window();

			// vst3_abstract_window override
			abstract_vst3_view *create_vst3_view_instance() override;
			void resize(const unsigned int width, const unsigned int height) override;
			void open(void *parent_window) override;
			void close(void) override;
	
			// abstract_window override
			void system_redraw_rect(const rectangle& rect) override;

			//	widget override
			void redraw(void) override;
		private:
			static LRESULT CALLBACK windowProc(HWND window, UINT msg, WPARAM w_param, LPARAM l_param);
			HWND m_plugin_window;
			bool m_has_focus;
		};


		class win32_vst3_view : public abstract_vst3_view {

		public:
			win32_vst3_view(win32_vst3_window *window);
			~win32_vst3_view();

			//	CPluginView override
			Steinberg::tresult PLUGIN_API isPlatformTypeSupported(Steinberg::FIDString type) SMTG_OVERRIDE;
			void attachedToParent() SMTG_OVERRIDE;
			void removedFromParent() SMTG_OVERRIDE;
		};

	} /* View */
} /* Gammou */




#endif // _WIN32

#endif /* */