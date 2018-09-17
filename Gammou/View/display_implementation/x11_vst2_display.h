#ifndef X11_VST2_DISPLAY_H_
#define X11_VST2_DISPLAY_H_

#include "display/abstract_x11_display.h"
#include "display/abstract_vst2_display.h"

namespace Gammou {

    namespace View {

        class x11_vst2_display : 
            public abstract_x11_display,
            public abstract_vst2_display {

        public:
            x11_vst2_display(View::widget& root_widget);
            ~x11_vst2_display();

            void open(void *host_parent_window) override;
        };  

    } /* View */

} /* Gammou */

#endif