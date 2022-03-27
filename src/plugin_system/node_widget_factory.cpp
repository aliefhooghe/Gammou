
#include <llvm/Transforms/Utils/Cloning.h>
#include <llvm/Linker/Linker.h>

#include <DSPJIT/log.h>

#include "node_widget_factory.h"

namespace Gammou {

    /**
     *  Plugin implementation
     */
    node_widget_factory::plugin::plugin(plugin_id id, const std::string& name, const std::string& category)
    : _id{id}, _name{name}, _category{category}
    {

    }

    std::unique_ptr<plugin_node_widget> node_widget_factory::plugin::create_node(
        abstract_configuration_directory& dir, const nlohmann::json&)
    {
        return create_node(dir);
    }


    /*
     *  Factory Implementation
     */
    node_widget_factory::node_widget_factory(llvm::LLVMContext& llvm_context)
    :   _llvm_context{llvm_context}
    {
        _module = std::make_unique<llvm::Module>("FACTORY", _llvm_context);
    }

    void node_widget_factory::register_plugin(std::unique_ptr<plugin>&& plugin)
    {
        auto plugin_module = plugin->module();
        auto id = plugin->id();

        if (plugin_module)
            add_library_module(std::move(plugin_module));

        _plugins.emplace(id, std::move(plugin));
    }

    void node_widget_factory::add_library_module(std::unique_ptr<llvm::Module>&& m)
    {
        llvm::Linker::linkModules(*_module, std::move(m));
    }

    std::unique_ptr<plugin_node_widget> node_widget_factory::create_node(plugin_id id, abstract_configuration_directory& parent_config)
    {
        if (id == no_id) {
            return {};
        }
        else {
            auto it = _plugins.find(id);
            if (it != _plugins.end())
                return it->second->create_node(parent_config);
            else
                throw std::runtime_error("node_widget_factory::create_node unkown id");
        }
    }

    std::unique_ptr<plugin_node_widget> node_widget_factory::create_node(const nlohmann::json& state, abstract_configuration_directory& parent_config)
    {
        const auto uid = state.at("plugin-uid").get<uint64_t>();

        LOG_DEBUG("node_widget_factory::create_node({plugin-uid : %llu, ...})\n", uid);
        auto it = _plugins.find(uid);
        if (it == _plugins.end())
            throw std::runtime_error("node_widget_factory::create_node unkown id");

        // Use state if one was provided
        auto internal_state_it = state.find("state");
        if (internal_state_it != state.end())
            return it->second->create_node(parent_config, *internal_state_it);
        else
            return it->second->create_node(parent_config);
    }

    std::unique_ptr<llvm::Module> node_widget_factory::module()
    {
        return llvm::CloneModule(*_module);
    }

    /*
     *  Plugin node widget implementation
     */

    plugin_node_widget::plugin_node_widget(
        const std::string& name,
        plugin_id id,
        std::unique_ptr<DSPJIT::compile_node_class>&& node)
    :   owning_node_widget{name, std::move(node)},
        _plugin_id{id}
    {

    }

    nlohmann::json plugin_node_widget::serialize()
    {
        nlohmann::json json;
        json["plugin-uid"] = _plugin_id;

        // Add the internal state if any
        auto internal_state = serialize_internal_state();
        if (!internal_state.is_null())
            json["state"] = serialize_internal_state();

        return json;
    }
}