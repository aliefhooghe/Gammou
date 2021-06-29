
#include <algorithm>

#include "helpers/filesystem_directory_model.h"
#include "ir_loader.h"
#include "node_widget_external_plugin.h"
#include "utils/wav_loader.h"
#include "backends/common/configuration.h"
#include "utils/serialization_helpers.h"

namespace Gammou {

    /**
     *  external widget node
     */
    class external_node_widget : public plugin_node_widget
    {
    public:
        using static_chunk_type = node_widget_external_plugin::static_chunk_type;

        external_node_widget(
            const std::string& name, plugin_id id,
            std::unique_ptr<DSPJIT::compile_node_class>&& node,
            abstract_configuration_directory& parent_config,
            static_chunk_type chunk_type)
        :   plugin_node_widget{name, id, std::move(node)},
            _chunk_type{chunk_type}
        {
            LOG_INFO("CREATE external_node_widget with wav channel\n");
            _initialize(parent_config, chunk_type);
        }

        ~external_node_widget() noexcept = default;

        nlohmann::json serialize_internal_state() override
        {
            if (_sample_path.has_value()) {
                nlohmann::json json = {{"sample-path", _sample_path.value()}};
                if (_chunk_type == static_chunk_type::WAV_CHANNEL)
                    json["channel-id"] = _sample_channel_id;
                return json;
            }
            else {
                return {};
            }
        }

        void deserialize_internal_state(const nlohmann::json& json)
        {
            // Retrieve fields from json
            auto it = json.find("sample-path");

            if (it != json.end()) {
                _sample_path = it->get<std::string>();
                if (_chunk_type == static_chunk_type::WAV_CHANNEL)
                    json.at("channel-id").get_to(_sample_channel_id);
                _load_wav_sample(_sample_path.value());
            }
            // else : no sample to load
        }

    private:
        void _load_wav_sample(const std::filesystem::path& sample_path)
        {
            try {
                LOG_INFO("[external_node_widget] Loading wav sample '%s'\n",
                    sample_path.generic_string().c_str());
                const auto sample = load_wav_from_file(sample_path);

                //  Register the relevant static memory chunk
                if (_chunk_type == static_chunk_type::WAV_CHANNEL) {
                    _config_page->register_static_memory_chunk(node(), sample.clone_channel_data(0u));
                    _config_page->compile();
                    _sample_path = sample_path;
                }
                else {
                    // TOTO : Not implemented
                }
            }
            catch (const std::exception& e)
            {
                LOG_ERROR("[external_node_widget] Unable to load wav sample : '%s'\n", e.what());
            }
        }

        void _initialize(abstract_configuration_directory& parent_config, static_chunk_type chunk_type)
        {
            /// Create the configuration widget

            // only wav channel now
            if (chunk_type != static_chunk_type::WAV_CHANNEL)
                throw std::invalid_argument("external_node_widget : unsupported static memory chunk type");

            auto samples_browser =
                std::make_unique<View::filesystem_view>(
                    configuration::get_samples_path(),
                    100, 100);
            // Sample selection callback
            samples_browser->set_value_select_callback(
                [this](const auto& sample_path)
                {
                    _load_wav_sample(sample_path);
                });

            auto update_button = std::make_unique<View::text_push_button>("Update");
            update_button->set_callback(
                [br = samples_browser.get()]()
                {
                    br->update();
                });

            _config_widget =
                std::make_shared<View::header>(
                    View::make_vertical_layout(
                        std::move(update_button),
                        std::move(samples_browser)));

            /// Create the configuration directory
            auto new_name = name();
            _config_page = parent_config.create_page(new_name, _config_widget);
            set_name(new_name);
        }

        const static_chunk_type _chunk_type;
        std::optional<std::filesystem::path> _sample_path{};
        unsigned int _sample_channel_id{0u};
        std::unique_ptr<abstract_configuration_page> _config_page{};
        std::shared_ptr<View::widget> _config_widget{};
    };

    /**
     *  Plugin descriptor deserialization
     */
    static node_widget_external_plugin::static_chunk_type _parse_chunk_type(const std::string& str)
    {
        if (str == "wav-channel")
            return  node_widget_external_plugin::static_chunk_type::WAV_CHANNEL;
        else if (str == "wav-sample")
            return  node_widget_external_plugin::static_chunk_type::WAV_SAMPLE;
        else
            throw std::invalid_argument("Unknown static memory chunk type");
    }

    void from_json(
        const nlohmann::json& j,
        node_widget_external_plugin::descriptor& desc)
    {
        j.at("name").get_to(desc.name);
        j.at("uid").get_to(desc.plugin_id);
        j.at("category").get_to(desc.category);
        j.at("input-names").get_to(desc.input_names);
        j.at("output-names").get_to(desc.output_names);
        j.at("modules").get_to(desc.modules_paths);

        // static chunk type is optional
        auto it = j.find("static-chunk-type");
        if (it != j.end())
            desc.static_chunk = _parse_chunk_type(it->get<std::string>());
    }

    /**
     *  external node widget plugin implementation
     */

    node_widget_external_plugin::node_widget_external_plugin(
        const node_widget_factory::plugin_id plugin_id,
        const std::string& name, const std::string& category,
        static_chunk_type static_memory,
        std::unique_ptr<llvm::Module>&& module)
    :   node_widget_factory::plugin{plugin_id, name, category},
        _dsp_plugin{std::move(module)},
        _static_memory_chunk{static_memory}
    {
    }

    std::unique_ptr<node_widget_external_plugin> node_widget_external_plugin::from_desc(const node_widget_external_plugin::descriptor& desc, llvm::LLVMContext& ctx)
    {
        auto module = load_ir_modules(ctx, desc.modules_paths);
        auto plugin = std::make_unique<node_widget_external_plugin>(desc.plugin_id, desc.name, desc.category, desc.static_chunk, std::move(module));
        plugin->set_input_names(std::vector<std::string>(desc.input_names));
        plugin->set_input_names(std::vector<std::string>(desc.input_names));
        return plugin;
    }

    std::unique_ptr<plugin_node_widget> node_widget_external_plugin::create_node(abstract_configuration_directory& parent_config)
    {
        const auto& proc_info = _dsp_plugin.get_process_info();
        //  Create a node widget, with static chunk support if needed
        std::unique_ptr<plugin_node_widget> node =
            proc_info.use_static_memory ?
                std::make_unique<external_node_widget>(
                    name(), id(), _dsp_plugin.create_node(), parent_config, _static_memory_chunk) :
                std::make_unique<plugin_node_widget>(name(), id(),  _dsp_plugin.create_node());
        _set_io_names(*node);
        return node;
    }

    std::unique_ptr<plugin_node_widget> node_widget_external_plugin::create_node(abstract_configuration_directory& parent_config, const nlohmann::json& internal_state)
    {
        const auto& proc_info = _dsp_plugin.get_process_info();

        if (proc_info.use_static_memory) {
            auto node =
                std::make_unique<external_node_widget>(
                    name(), id(), _dsp_plugin.create_node(), parent_config, _static_memory_chunk);
            node->deserialize_internal_state(internal_state);
            _set_io_names(*node);
            return node;
        }
        else {
            return create_node(parent_config);
        }
    }

    std::unique_ptr<llvm::Module> node_widget_external_plugin::module()
    {
        return _dsp_plugin.create_module();
    }

    void node_widget_external_plugin::set_input_names(std::vector<std::string>&& names)
    {
        const auto& proc_info = _dsp_plugin.get_process_info();
        if (names.size() == proc_info.input_count)
            _node_input_names = std::move(names);
    }

    void node_widget_external_plugin::set_output_names(std::vector<std::string>&& names)
    {
        const auto& proc_info = _dsp_plugin.get_process_info();
        if (names.size() == proc_info.output_count)
            _node_output_names = std::move(names);
    }

    void node_widget_external_plugin::_set_io_names(plugin_node_widget& widget)
    {
        for (auto i = 0u; i < _node_input_names.size(); ++i)
            widget.set_input_name(i, _node_input_names[i]);

        for (auto i = 0; i < _node_output_names.size(); ++i)
            widget.set_output_name(i, _node_output_names[i]);
    }
}