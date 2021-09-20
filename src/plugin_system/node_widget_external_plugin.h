#ifndef GAMMOU_NODE_WIDGET_EXTERNAL_PLUGIN_H_
#define GAMMOU_NODE_WIDGET_EXTERNAL_PLUGIN_H_

#include "node_widget_factory.h"
#include "external_plugin/external_plugin.h" // DSPJIT

#include <filesystem>
#include <nlohmann/json.hpp>

namespace Gammou
{
    class node_widget_external_plugin : public node_widget_factory::plugin {
    public:
        enum class static_chunk_type
        {
            WAV_CHANNEL,
            WAV_SAMPLE
        };

        struct descriptor
        {
            node_widget_factory::plugin_id plugin_id{0u};
            std::string name{};
            std::string category{};
            std::vector<std::string> input_names{};
            std::vector<std::string> output_names{};
            std::vector<std::filesystem::path> modules_paths{};
            static_chunk_type static_chunk{static_chunk_type::WAV_CHANNEL};
        };

        node_widget_external_plugin(
            const node_widget_factory::plugin_id plugin_id,
            const std::string& name, const std::string& category,
            const static_chunk_type static_memory,
            std::unique_ptr<llvm::Module>&& module);

        node_widget_external_plugin(const node_widget_external_plugin&) = delete;
        node_widget_external_plugin(node_widget_external_plugin&&) = default;
        ~node_widget_external_plugin() = default;

        static std::unique_ptr<node_widget_external_plugin> from_desc(const descriptor&, llvm::LLVMContext&);

        std::unique_ptr<plugin_node_widget> create_node(abstract_configuration_directory&) override;
        std::unique_ptr<plugin_node_widget> create_node(abstract_configuration_directory&, const nlohmann::json&) override;
        std::unique_ptr<llvm::Module> module() override;

        void set_input_names(std::vector<std::string>&& names);
        void set_output_names(std::vector<std::string>&& names);

    private:
        void _set_io_names(plugin_node_widget& widget);

        DSPJIT::external_plugin _dsp_plugin;
        static_chunk_type _static_memory_chunk{};
        std::vector<std::string> _node_input_names{};
        std::vector<std::string> _node_output_names{};
    };

    /**
     * \brief Deserialize a node widget external plugin descriptor from a json object
     */
    void from_json(const nlohmann::json&, node_widget_external_plugin::descriptor&);
}

#endif