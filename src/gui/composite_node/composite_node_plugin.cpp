
#include <DSPJIT/composite_node.h>
#include <DSPJIT/log.h>

#include "composite_node_plugin.h"
#include "composite_node_widget.h"

namespace Gammou {

    composite_node_plugin::composite_node_plugin(factory_widget& factory)
    :   node_widget_factory::plugin{
            composite_node_plugin::uid,
            "Circuit",
            "Composite"},
        _factory{factory}
    {
    }

    std::unique_ptr<plugin_node_widget> composite_node_plugin::create_node(abstract_configuration_directory& parent_config)
    {
        return std::make_unique<composite_node_widget>(_factory, parent_config, 2, 2);
    }

    std::unique_ptr<plugin_node_widget> composite_node_plugin::create_node(abstract_configuration_directory& parent_config, const nlohmann::json& json)
    {
        composite_node_widget::state state;
        from_json(json, state);
        return std::make_unique<composite_node_widget>(_factory, parent_config, state);
    }
}