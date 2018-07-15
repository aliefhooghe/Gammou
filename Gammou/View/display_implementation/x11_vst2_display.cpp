
#include "x11_vst2_display.h"

namespace Gammou {

    namespace View {

        x11_vst2_display::x11_vst2_display(
            View::widget& root_widget)
        :   abstract_display(root_widget),
            abstract_x11_display(root_widget),
            abstract_vst2_display(root_widget)
        {
        }
        
        x11_vst2_display::~x11_vst2_display()
        {
        }

        void x11_vst2_display::open(void *host_parent_window)
        {
            create_window((Window)host_parent_window, "");
        }

        void x11_vst2_display::close()
        {
            abstract_x11_display::close();
        }

    } /* View */

} /* Gammou */