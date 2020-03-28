#ifndef GAMMOU_NODE_WIDGET_FACTORY_H_
#define GAMMOU_NODE_WIDGET_FACTORY_H_

#include <cstdint>
#include <unordered_map>
#include <functional>

#include "gui/circuit_editor.h"

namespace Gammou {

    class node_widget_plugin {
    public:
        virtual std::unique_ptr<node_widget> create_node() = 0;
        virtual std::unique_ptr<llvm::Module> module() { return nullptr; }
    };

    class node_widget_factory {
    public:
        using uid = uint64_t;

        node_widget_factory(llvm::LLVMContext&);

        void register_plugin(uid id, std::unique_ptr<node_widget_plugin>&& plugin);
        std::unique_ptr<llvm::Module> module();
        std::unique_ptr<node_widget> create_node(uid id);

        auto begin() const noexcept { return _plugins.begin(); }
        auto end() const noexcept { return _plugins.end(); }
    private:
        llvm::LLVMContext& _llvm_context;
        std::unordered_map<uid, std::unique_ptr<node_widget_plugin>> _plugins{};
        std::unique_ptr<llvm::Module> _module{};
    };

}

#endif