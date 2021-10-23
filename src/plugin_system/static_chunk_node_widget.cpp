
#include <DSPJIT/log.h>

#include "backends/common/default_configuration.h"
#include "helpers/layout_builder.h"
#include "static_chunk_node_widget.h"
#include "utils/serialization_helpers.h"
#include "utils/wav_loader.h"

namespace Gammou
{

    static_chunk_node_widget::static_chunk_node_widget(
        const std::string &name, plugin_id id,
        std::unique_ptr<DSPJIT::compile_node_class> &&node,
        abstract_configuration_directory &parent_config,
        static_chunk_type chunk_type)
        : plugin_node_widget{name, id, std::move(node)},
          _chunk_type{chunk_type}
    {
        LOG_DEBUG("CREATE external_node_widget with wav channel\n");
        _initialize(parent_config);
    }

    static_chunk_node_widget::~static_chunk_node_widget() noexcept
    {
        // Free the allocated static chunk if any
        _config_page->free_static_memory_chunk(node());
    }

    nlohmann::json static_chunk_node_widget::serialize_internal_state()
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

    void static_chunk_node_widget::deserialize_internal_state(const nlohmann::json &json)
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

    bool static_chunk_node_widget::_load_wav_sample(const std::filesystem::path &sample_path)
    {
        try
        {
            LOG_INFO("[external_node_widget] Loading wav sample '%s'\n",
                     sample_path.generic_string().c_str());

            //  Register the relevant static memory chunk
            if (_chunk_type == static_chunk_type::WAV_CHANNEL) {
                const auto sample = load_wav_from_file(sample_path);
                _config_page->register_static_memory_chunk(node(), sample.clone_channel_data(0u));
                _config_page->compile();
                _sample_path = sample_path;
                return true;
            }
            else {
                // TOTO : Not implemented
                LOG_WARNING("[external_node_widget] Unsupported chunk type\n");
                return false;
            }
        }
        catch (const std::exception &e)
        {
            LOG_ERROR("[external_node_widget] Unable to load wav sample : '%s'\n", e.what());
            return false;
        }
    }

    void static_chunk_node_widget::_initialize(abstract_configuration_directory &parent_config)
    {
        /// Create the configuration widget

        // only wav channel now
        if (_chunk_type != static_chunk_type::WAV_CHANNEL)
            throw std::invalid_argument("external_node_widget : unsupported static memory chunk type");

        auto samples_browser =
            std::make_unique<View::filesystem_view>(
                default_configuration::get_samples_path(),
                100, 100);
        // Sample selection callback
        samples_browser->set_value_select_callback(
            [this, br = samples_browser.get()](const auto &sample_path)
            {
                if (!_load_wav_sample(sample_path))
                    br->reset_selection();
            });

        auto update_button = std::make_unique<View::text_push_button>("Update");
        update_button->set_callback(
            [br = samples_browser.get()]()
            {
                br->update();
            });

        const View::layout_builder builder{};
        auto config_widget = builder.shared_header(
            builder.vertical(std::move(update_button), std::move(samples_browser)));

        /// Create the configuration directory
        auto new_name = name();
        _config_page = parent_config.create_page(new_name, std::move(config_widget));
        set_name(new_name);
    }

} /* namespace Gammou */