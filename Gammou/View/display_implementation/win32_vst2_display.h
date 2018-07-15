#ifndef WIN32_VST2_DISPLAY_H_
#define WIN32_VST2_DISPLAY_H_

#include "display/abstract_vst2_display.h"
#include "display/abstract_win32_display.h"

namespace Gammou {

	namespace View {

		class win32_vst2_display :
			public abstract_win32_display,
			public abstract_vst2_display {

		public:
			win32_vst2_display(View::widget& root_widget);
			~win32_vst2_display();

			void open(void *host_parent_window) override;
			void close() override;
		};

	} /* View */

}	/* Gammou */

#endif