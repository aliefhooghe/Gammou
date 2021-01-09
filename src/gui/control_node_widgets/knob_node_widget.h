#ifndef GAMMOU_KNOB_NODE_WIDGET_H_
#define GAMMOU_KNOB_NODE_WIDGET_H_

#include "synthesizer/synthesizer.h"
#include "plugin_system/node_widget_factory.h"

namespace Gammou {

    class  knob_node_widget_plugin  : public node_widget_factory::plugin {
    public:
        knob_node_widget_plugin(synthesizer& synth);
        std::unique_ptr<plugin_node_widget> create_node(circuit_tree_model&) override;
        std::unique_ptr<plugin_node_widget> create_node(circuit_tree_model&, const nlohmann::json&) override;
    private:
        synthesizer& _synth;
    };

}

#endif