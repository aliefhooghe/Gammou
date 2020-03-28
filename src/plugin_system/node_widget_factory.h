#ifndef GAMMOU_NODE_WIDGET_FACTORY_H_
#define GAMMOU_NODE_WIDGET_FACTORY_H_

#include <cstdint>
#include <unordered_map>
#include <functional>

#include "gui/circuit_editor.h"

namespace Gammou {

    class node_widget_plugin {
    public:
        virtual ~node_widget_plugin() = default;
        virtual std::unique_ptr<node_widget> create_node() = 0;
        virtual std::unique_ptr<llvm::Module> module() { return nullptr; }

        const auto& get_category() const noexcept { return _category; }
        const auto& get_name() const noexcept { return _name; }

    protected:
        std::string _category{"Unknown"};
        std::string _name{"Unamed"};
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

        auto& get_llvm_context() noexcept { return _llvm_context; }
    private:
        llvm::LLVMContext& _llvm_context;
        std::unordered_map<uid, std::unique_ptr<node_widget_plugin>> _plugins{};
        std::unique_ptr<llvm::Module> _module{};
    };

}

#endif