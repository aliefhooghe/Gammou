#ifndef GAMMOU_BUILTIN_PLUGIN_H_
#define GAMMOU_BUILTIN_PLUGIN_H_

#include "plugin_system/node_widget_factory.h"

namespace Gammou {

    template <typename TCompileNode>
    class node_widget_builtin_plugin : public node_widget_factory::plugin {
    public:
        using plugin_id = node_widget_factory::plugin_id;

        node_widget_builtin_plugin(
            plugin_id id,
            const std::string& name,
            const std::string& category)
        :   node_widget_factory::plugin{id, name, category}
        {}

        std::unique_ptr<plugin_node_widget> create_node(abstract_configuration_directory&) override
        {
            return std::make_unique<plugin_node_widget>(
                name(), id(),
                std::make_unique<TCompileNode>());
        }
    };

    template <typename TCompileNode>
    auto make_builtin_plugin(
        node_widget_factory::plugin_id id,
        const std::string& name,
        const std::string& category)
    {
        return std::make_unique<node_widget_builtin_plugin<TCompileNode>>(id, name, category);
    }

}

#endif