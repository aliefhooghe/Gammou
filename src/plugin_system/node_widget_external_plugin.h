#ifndef GAMMOU_NODE_WIDGET_EXTERNAL_PLUGIN_H_
#define GAMMOU_NODE_WIDGET_EXTERNAL_PLUGIN_H_

#include "node_widget_factory.h"
#include "external_plugin.h" // DSPJIT

#include <filesystem>
#include <nlohmann/json.hpp>

namespace nlohmann {
    /**
     * This specialization in the nlohmann namespace is needed because we don't want to write  
     * in the std namespace. 
     * 
     * see github.com/nlohmann/json#how-do-i-convert-third-party-types
     */
    template <>
    struct adl_serializer<std::filesystem::path> {
        static void to_json(json& j, const std::filesystem::path& path)
        {
            j = path.string();
        }

        static void from_json(const json& j, std::filesystem::path& path)
        {
            path = j.get<std::string>();
        }
    };
}

namespace Gammou
{
    struct node_widget_external_plugin_descriptor {
        node_widget_factory::plugin_id plugin_id{0u};
        std::string name{};
        std::string category{};
        std::vector<std::string> input_names{};
        std::vector<std::string> output_names{};
        std::vector<std::filesystem::path> modules_paths{};
    };

    void from_json(const nlohmann::json&, node_widget_external_plugin_descriptor&);

    class node_widget_external_plugin : public node_widget_factory::plugin {
    public:
        node_widget_external_plugin(
            llvm::LLVMContext &llvm_context,
            const node_widget_external_plugin_descriptor& desc);
        ~node_widget_external_plugin() = default;

        node_widget_external_plugin(const node_widget_external_plugin&) = delete;
        node_widget_external_plugin(node_widget_external_plugin&&) = default;

        std::unique_ptr<plugin_node_widget> create_node(circuit_tree&) override;
        std::unique_ptr<llvm::Module> module() override;

    private:
        DSPJIT::external_plugin _dsp_plugin;
        std::vector<std::string> _node_input_names{};
        std::vector<std::string> _node_output_names{};
    };

}

#endif