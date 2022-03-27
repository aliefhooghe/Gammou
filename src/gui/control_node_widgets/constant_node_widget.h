#ifndef GAMMOU_CONSTANT_WIDGET_H_
#define GAMMOU_CONSTANT_WIDGET_H_

#include "plugin_system/node_widget_factory.h"

namespace Gammou {

    class constant_node_widget_plugin  : public node_widget_factory::plugin {
    public:
        constant_node_widget_plugin();
        std::unique_ptr<plugin_node_widget> create_node(abstract_configuration_directory&) override;
        std::unique_ptr<plugin_node_widget> create_node(abstract_configuration_directory&, const nlohmann::json&) override;
    };

}

#endif