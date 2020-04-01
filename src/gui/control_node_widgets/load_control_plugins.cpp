
#include "load_control_plugins.h"

#include "knob_node_widget.h"

namespace Gammou {

    void load_control_plugins(synthesizer& synth, node_widget_factory& factory)
    {
        factory.register_plugin(1024, std::make_unique<knob_node_widget_plugin>(synth));
    }
}