#ifndef GAMMOU_NODE_WIDGET_FACTORY_H_
#define GAMMOU_NODE_WIDGET_FACTORY_H_

#include <cstdint>
#include <unordered_map>
#include <functional>
#include <nlohmann/json.hpp>

#include "gui/circuit_editor.h"
#include "gui/circuit_tree_model.h"

namespace Gammou {

    class plugin_node_widget;

    class node_widget_factory {
    public:
        using plugin_id = uint64_t;
        static constexpr plugin_id no_id = 0u;

        /**
         *  \class plugin
         *  \brief
         */
        class plugin {
        public:
            virtual ~plugin() = default;

            plugin(
                plugin_id id,
                const std::string name,
                const std::string category)
            :   _id{id}, _name{name}, _category{category}
            {}

            /**
             *  \return a new plugin node built with default state
             */
            virtual std::unique_ptr<plugin_node_widget> create_node(circuit_tree& dir) =0;

            /**
             *  \param internal_state from which the node will be deserialized
             *  \return a new plugin node built with given state
             */
            virtual std::unique_ptr<plugin_node_widget> create_node(
                circuit_tree& dir,
                const nlohmann::json& internal_state)
            {
                (void)internal_state;
                return create_node(dir);
            }

            /**
             *  \brief Return a module containing all the dependendies needed
             *      to compile the nodes created by the plugin
             */
            virtual std::unique_ptr<llvm::Module> module() { return nullptr; }

            const auto id() const noexcept { return _id; }
            const auto& name() const noexcept { return _name; }
            const auto& category() const noexcept { return _category; }
        private:
            const plugin_id _id;
            const std::string _name;
            const std::string _category;
        };

        /**
         *
         */
        node_widget_factory(llvm::LLVMContext&);

        /**
         *  \brief Register a plugin into the factory.
         *  \param plugin the new plugin to be registered
         */
        void register_plugin(std::unique_ptr<plugin>&& plugin);

        /**
         *  \brief Add a plugin dependency module into the factory
         *  \param m the module to be added
         */
        void add_library_module(std::unique_ptr<llvm::Module>&& m);

        /**
         *  \return a module where all registred plugins dependencies are linked
         */
        std::unique_ptr<llvm::Module> module();

        /**
         *  \brief create a node with the plugin identified by the plugin id
         *  \param id the plugin id
         *  \return the new node
         */
        std::unique_ptr<plugin_node_widget> create_node(
            plugin_id id,
            circuit_tree& parent_config);

        /**
         *  \brief create a node with the plugin identified by the plugin 
         *  \param state state to build the node from
         *  \return the new node
         */
        std::unique_ptr<plugin_node_widget> create_node(
            const nlohmann::json& state,
            circuit_tree& parent_config);

        /**
         *
         */
        auto begin() const noexcept { return _plugins.begin(); }

        /**
         *
         */
        auto end() const noexcept { return _plugins.end(); }

        /**
         *
         */
        auto& get_llvm_context() noexcept { return _llvm_context; }
    private:
        llvm::LLVMContext& _llvm_context;
        std::unordered_map<plugin_id, std::unique_ptr<plugin>> _plugins{};
        std::unique_ptr<llvm::Module> _module{};
    };

    class plugin_node_widget : public owning_node_widget {
    public:
        using plugin_id = node_widget_factory::plugin_id;

        plugin_node_widget(
            const std::string& name,
            plugin_id id,
            std::unique_ptr<DSPJIT::compile_node_class>&& node);

        /**
         * @return factory plugin id
         */
        auto id() const noexcept { return _plugin_id; }
        
        /**
         * @return a json object with factory id and internal state
         */
        nlohmann::json serialize() override;

    protected:
        virtual nlohmann::json serialize_internal_state() { return {}; }

    private:
        const plugin_id _plugin_id;
    };

}

#endif