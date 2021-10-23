#ifndef GAMMOU_NODE_WIDGET_EXTERNAL_PLUGIN_H_
#define GAMMOU_NODE_WIDGET_EXTERNAL_PLUGIN_H_

#include "node_widget_factory.h"
#include <DSPJIT/external_plugin.h>

#include <filesystem>
#include <nlohmann/json.hpp>

namespace Gammou
{
    /**
     * \brief Plugin implementation for node whose process is defined in external bitcode.
     * It provide the necessary user interface for static chunk management (sample browsing and loading)
     */
    class external_plugin : public node_widget_factory::plugin {
    public:

        /**
         * \brief Define different sources of data for the static memory chunk if used
         * These map to a given memory layout that the nodes should handle.
         */
        enum class static_chunk_type
        {
            NONE,           //<< No static chunk is used
            WAV_CHANNEL,    //<< Data from a wav single channel is used as static chunk
            // WAV_SAMPLE      //<< Data from a wav's all channels is used as a static chunk
        };

        struct descriptor
        {
            node_widget_factory::plugin_id plugin_id{0u};
            std::string name{};
            std::string category{};
            std::vector<std::string> input_names{};
            std::vector<std::string> output_names{};
            std::vector<std::filesystem::path> modules_paths{};
            static_chunk_type static_chunk{static_chunk_type::NONE};
        };

        external_plugin(
            const node_widget_factory::plugin_id plugin_id,
            const std::string& name, const std::string& category,
            const static_chunk_type static_memory,
            std::unique_ptr<llvm::Module>&& module);

        external_plugin(const external_plugin&) = delete;
        external_plugin(external_plugin&&) = default;
        ~external_plugin() = default;

        static std::unique_ptr<external_plugin> from_desc(const descriptor&, llvm::LLVMContext&);

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
    void from_json(const nlohmann::json&, external_plugin::descriptor&);
}

#endif