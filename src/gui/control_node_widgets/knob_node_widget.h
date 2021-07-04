#ifndef GAMMOU_KNOB_NODE_WIDGET_H_
#define GAMMOU_KNOB_NODE_WIDGET_H_

#include "synthesizer/synthesizer.h"
#include "plugin_system/node_widget_factory.h"

namespace Gammou {

    class  value_knob_node_widget_plugin  : public node_widget_factory::plugin {
    public:
        value_knob_node_widget_plugin(synthesizer& synth);
        std::unique_ptr<plugin_node_widget> create_node(abstract_configuration_directory&) override;
        std::unique_ptr<plugin_node_widget> create_node(abstract_configuration_directory&, const nlohmann::json&) override;
    private:
        synthesizer& _synth;
    };

    class  gain_knob_node_widget_plugin  : public node_widget_factory::plugin {
    public:
        gain_knob_node_widget_plugin(synthesizer& synth);
        std::unique_ptr<plugin_node_widget> create_node(abstract_configuration_directory&) override;
        std::unique_ptr<plugin_node_widget> create_node(abstract_configuration_directory&, const nlohmann::json&) override;
    private:
        synthesizer& _synth;
    };

}

#endif