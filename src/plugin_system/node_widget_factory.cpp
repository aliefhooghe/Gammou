
#include <llvm/Transforms/Utils/Cloning.h>
#include <llvm/Linker/Linker.h>

#include "node_widget_factory.h"

namespace Gammou {

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
            add_module(std::move(plugin_module));

        _plugins.emplace(id, std::move(plugin));
    }

    void node_widget_factory::add_module(std::unique_ptr<llvm::Module>&& m)
    {
        llvm::Linker::linkModules(*_module, std::move(m));
    }

    std::unique_ptr<plugin_node_widget> node_widget_factory::create_node(plugin_id id)
    {
        auto it = _plugins.find(id);
        if (it != _plugins.end())
            return it->second->create_node();
        else
            throw std::runtime_error("node_widget_factory::create_node unkown id");
    }

    std::unique_ptr<plugin_node_widget> node_widget_factory::create_node(plugin_id id, const nlohmann::json& internal_state)
    {
        auto it = _plugins.find(id);
        if (it != _plugins.end())
            return it->second->create_node(internal_state);
        else
            throw std::runtime_error("node_widget_factory::create_node unkown id");
    }

    std::unique_ptr<llvm::Module> node_widget_factory::module()
    {
        return llvm::CloneModule(*_module);
    }

    //

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
        json["state"] = serialize_internal_state();
        return json;
    }
}