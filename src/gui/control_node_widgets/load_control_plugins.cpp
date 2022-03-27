
#include "load_control_plugins.h"

#include "knob_node_widget.h"
#include "constant_node_widget.h"

namespace Gammou {

    void load_control_plugins(synthesizer& synth, node_widget_factory& factory)
    {
        factory.register_plugin(std::make_unique<value_knob_node_widget_plugin>(synth));
        factory.register_plugin(std::make_unique<gain_knob_node_widget_plugin>(synth));
        factory.register_plugin(std::make_unique<constant_node_widget_plugin>());
    }
}