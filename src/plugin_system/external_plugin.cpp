
#include <algorithm>

#include "external_plugin.h"
#include "helpers/filesystem_directory_model.h"
#include "ir_loader.h"
#include "utils/serialization_helpers.h"
#include "static_chunk_node_widget.h"

namespace Gammou {


    /**
     *  Plugin descriptor deserialization
     */
    static external_plugin::static_chunk_type _parse_chunk_type(const std::string& str)
    {
        if (str == "wav-channel")
            return  external_plugin::static_chunk_type::WAV_CHANNEL;
        // else if (str == "wav-sample")
        //     return  external_plugin::static_chunk_type::WAV_SAMPLE;
        else
            throw std::invalid_argument("Unknown static memory chunk type");
    }

    void from_json(
        const nlohmann::json& j,
        external_plugin::descriptor& desc)
    {
        j.at("name").get_to(desc.name);
        j.at("uid").get_to(desc.plugin_id);
        j.at("category").get_to(desc.category);
        j.at("input-names").get_to(desc.input_names);
        j.at("output-names").get_to(desc.output_names);
        j.at("modules").get_to(desc.modules_paths);

        // static chunk type is optional
        auto it = j.find("static-chunk-type");
        desc.static_chunk = it != j.end() ?
            _parse_chunk_type(it->get<std::string>()) :
            external_plugin::static_chunk_type::NONE;
    }

    /**
     *  external node widget plugin implementation
     */

    external_plugin::external_plugin(
        const node_widget_factory::plugin_id plugin_id,
        const std::string& name, const std::string& category,
        static_chunk_type static_memory,
        std::unique_ptr<llvm::Module>&& module)
    :   node_widget_factory::plugin{plugin_id, name, category},
        _dsp_plugin{std::move(module)},
        _static_memory_chunk{static_memory}
    {
        const auto& proc_info = _dsp_plugin.get_process_info();
        if (proc_info.use_static_memory && static_memory == static_chunk_type::NONE)
            throw std::invalid_argument("No chunk type was declared for an external node using static chunk");
    }

    std::unique_ptr<external_plugin> external_plugin::from_desc(const external_plugin::descriptor& desc, llvm::LLVMContext& ctx)
    {
        auto module = load_ir_modules(ctx, desc.modules_paths);
        auto plugin = std::make_unique<external_plugin>(desc.plugin_id, desc.name, desc.category, desc.static_chunk, std::move(module));
        plugin->set_input_names(std::vector<std::string>(desc.input_names));
        plugin->set_output_names(std::vector<std::string>(desc.output_names));
        return plugin;
    }

    std::unique_ptr<plugin_node_widget> external_plugin::create_node(abstract_configuration_directory& parent_config)
    {
        const auto& proc_info = _dsp_plugin.get_process_info();
        //  Create a node widget, with static chunk support if needed
        std::unique_ptr<plugin_node_widget> node =
            proc_info.use_static_memory ?
                std::make_unique<static_chunk_node_widget>(
                    name(), id(), _dsp_plugin.create_node(), parent_config, _static_memory_chunk) :
                std::make_unique<plugin_node_widget>(name(), id(),  _dsp_plugin.create_node());
        _set_io_names(*node);
        return node;
    }

    std::unique_ptr<plugin_node_widget> external_plugin::create_node(abstract_configuration_directory& parent_config, const nlohmann::json& internal_state)
    {
        const auto& proc_info = _dsp_plugin.get_process_info();

        if (proc_info.use_static_memory) {
            auto node =
                std::make_unique<static_chunk_node_widget>(
                    name(), id(), _dsp_plugin.create_node(), parent_config, _static_memory_chunk);
            node->deserialize_internal_state(internal_state);
            _set_io_names(*node);
            return node;
        }
        else {
            return create_node(parent_config);
        }
    }

    std::unique_ptr<llvm::Module> external_plugin::module()
    {
        return _dsp_plugin.create_module();
    }

    void external_plugin::set_input_names(std::vector<std::string>&& names)
    {
        const auto& proc_info = _dsp_plugin.get_process_info();
        if (names.size() == proc_info.input_count)
            _node_input_names = std::move(names);
    }

    void external_plugin::set_output_names(std::vector<std::string>&& names)
    {
        const auto& proc_info = _dsp_plugin.get_process_info();
        if (names.size() == proc_info.output_count)
            _node_output_names = std::move(names);
    }

    void external_plugin::_set_io_names(plugin_node_widget& widget)
    {
        for (auto i = 0u; i < _node_input_names.size(); ++i)
            widget.set_input_name(i, _node_input_names[i]);

        for (auto i = 0; i < _node_output_names.size(); ++i)
            widget.set_output_name(i, _node_output_names[i]);
    }
}