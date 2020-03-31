
#include <llvm/Transforms/Utils/Cloning.h>
#include <llvm/Linker/Linker.h>

#include "node_widget_factory.h"

namespace Gammou {

    node_widget_factory::node_widget_factory(llvm::LLVMContext& llvm_context)
    :   _llvm_context{llvm_context}
    {
        _module = std::make_unique<llvm::Module>("FACTORY", _llvm_context);
    }

    void node_widget_factory::register_plugin(uid id, std::unique_ptr<node_widget_plugin>&& plugin)
    {
        auto plugin_module = plugin->module();
        if (plugin_module)
            llvm::Linker::linkModules(*_module, std::move(plugin_module));
        _plugins.emplace(id, std::move(plugin));
    }

    std::unique_ptr<node_widget> node_widget_factory::create_node(uid id)
    {
        auto it = _plugins.find(id);
        if (it != _plugins.end())
            return it->second->create_node();
        else
            throw std::runtime_error("node_widget_factory::create_node unkown id");
    }

    std::unique_ptr<llvm::Module> node_widget_factory::module()
    {
        return llvm::CloneModule(*_module);
    }

}