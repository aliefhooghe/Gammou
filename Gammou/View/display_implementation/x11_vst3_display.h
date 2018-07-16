#ifndef X11_VST3_DISPLAY_H_
#define X11_VST3_DISPLAY_H_

#include "display/abstract_vst3_display.h"
#include "display/abstract_x11_display.h"

namespace Gammou {

    namespace View {

        class x11_vst3_display : 
			public abstract_x11_display,
			public abstract_vst3_display {

		public:
			x11_vst3_display(View::widget& root_widget);
			~x11_vst3_display();

			void open(void *host_parent_window) override;
			abstract_vst3_view *create_vst3_view_instance() override;
		};

		class x11_vst3_view : public abstract_vst3_view {

		public:
			x11_vst3_view(x11_vst3_display *display);
			~x11_vst3_view();

			//	CPluginView override
			Steinberg::tresult PLUGIN_API isPlatformTypeSupported(Steinberg::FIDString type) override;
		};

    } /* View */

} /* Gammou */

#endif