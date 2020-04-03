#ifndef GAMMOU_KNOB_NODE_WIDGET_H_
#define GAMMOU_KNOB_NODE_WIDGET_H_

#include "plugin_system/node_widget_factory.h"
#include "synthesizer/synthesizer.h"

namespace Gammou {

    class  knob_node_widget_plugin  : public node_widget_factory::plugin {
    public:
        knob_node_widget_plugin(synthesizer& synth);
        std::unique_ptr<plugin_node_widget> create_node() override;
        std::unique_ptr<plugin_node_widget> create_node(const nlohmann::json&) override;
    private:
        synthesizer& _synth;
    };

}

#endif