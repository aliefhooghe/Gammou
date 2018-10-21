
#include "gui_properties.h"

namespace Gammou {

    namespace Gui
    {

        namespace GuiProperties
        {
            void apply(View::knob &k)
            {
                k.set_on_color(knob_on_color);
                k.set_off_color(knob_off_color);
            }
        }

    }
} /* Gammou */