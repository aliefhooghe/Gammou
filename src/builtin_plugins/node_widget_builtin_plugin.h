#ifndef GAMMOU_BUILTIN_PLUGIN_H_
#define GAMMOU_BUILTIN_PLUGIN_H_

#include "plugin_system/node_widget_factory.h"

namespace Gammou {

    template <typename TCompileNode>
    class node_widget_builtin_plugin : public node_widget_plugin {
    public:
        node_widget_builtin_plugin(
            const std::string& name,
            const std::string& category)
        {
            _name = name;
            _category = category;
        }

        node_widget_builtin_plugin(
            const std::string& name,
            const std::string& category,
            const std::vector<std::string>& input_names,
            const std::vector<std::string>& output_names)
        :   node_widget_builtin_plugin{name, category}
        {
            _node_input_names = input_names;
            _node_output_names = output_names;
        }

        std::unique_ptr<node_widget> create_node() override
        {
            return std::make_unique<owning_node_widget>(
                _name,
                std::make_unique<TCompileNode>());
        }


    private:
        std::vector<std::string> _node_input_names{};
        std::vector<std::string> _node_output_names{};
    };

    template <typename TCompileNode>
    auto make_builtin_plugin(
        const std::string& name,
        const std::string& category)
    {
        return std::make_unique<node_widget_builtin_plugin<TCompileNode>>(name, category);
    }

    template <typename TCompileNode>
    auto make_builtin_plugin(
        const std::string& name,
        const std::string& category,
        const std::vector<std::string>& input_names,
        const std::vector<std::string>& output_names)
    {
        return std::make_unique<node_widget_builtin_plugin<TCompileNode>>(
            name, category, input_names, output_names);
    }

}

#endif