#ifndef WIN32_VST3_DISPLAY_H_
#define WIN32_VST3_DISPLAY_H_

#include "display/abstract_vst3_display.h"
#include "display/abstract_win32_display.h"

namespace Gammou {

	namespace View {

		class win32_vst3_display : 
			public abstract_win32_display,
			public abstract_vst3_display {

		public:
			win32_vst3_display(View::widget& root_widget);
			~win32_vst3_display();

			void open(void *host_parent_window) override;
			void close() override;
			abstract_vst3_view *create_vst3_view_instance() override;
		};

		class win32_vst3_view : public abstract_vst3_view {

		public:
			win32_vst3_view(win32_vst3_display *display);
			~win32_vst3_view();

			//	CPluginView override
			Steinberg::tresult PLUGIN_API isPlatformTypeSupported(Steinberg::FIDString type) override;
		};

	} /* View */



} /* Gammou */


#endif