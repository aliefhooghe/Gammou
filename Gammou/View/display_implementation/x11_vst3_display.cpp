
#include <cstring>
#include "x11_vst3_display.h"

namespace Gammou {

    namespace View {

        x11_vst3_display::x11_vst3_display(View::widget& root_widget)
        :	abstract_display(root_widget),
			abstract_x11_display(root_widget),
			abstract_vst3_display(root_widget)
        {
            DEBUG_PRINT("X11 Vst3 Display CTOR\n");
        }

        x11_vst3_display::~x11_vst3_display()
        {
        }

        void x11_vst3_display::open(void *host_parent_window)
        {
            create_window((Window)host_parent_window, "");
        }

        abstract_vst3_view *x11_vst3_display::create_vst3_view_instance()
        {
            return new x11_vst3_view(this);
        }

        //  x11_vst3_view implementation

        x11_vst3_view::x11_vst3_view(x11_vst3_display *display)
        :   abstract_vst3_view(display)
        {
            DEBUG_PRINT("X11 VST3 View ctor\n");
        }

        x11_vst3_view::~x11_vst3_view()
        {
            DEBUG_PRINT("X11 VST3 View DTOR\n");
        }

        Steinberg::tresult PLUGIN_API x11_vst3_view::isPlatformTypeSupported(
            Steinberg::FIDString type)
        {
            return (0 == std::strcmp(Steinberg::kPlatformTypeX11EmbedWindowID, type) ?
                Steinberg::kResultTrue : Steinberg::kResultFalse);
        }


    } /* View */

} /* Gammou */