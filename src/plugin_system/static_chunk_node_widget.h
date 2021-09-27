
#include "external_plugin.h"

namespace Gammou {

    /**
     * \brief A plugin node widget implementation
     */
    class static_chunk_node_widget : public plugin_node_widget
    {

    public:
        using static_chunk_type = external_plugin::static_chunk_type;

        static_chunk_node_widget(
            const std::string& name, plugin_id id,
            std::unique_ptr<DSPJIT::compile_node_class>&& node,
            abstract_configuration_directory& parent_config,
            static_chunk_type chunk_type);


        ~static_chunk_node_widget() noexcept override;

        nlohmann::json serialize_internal_state() override;
        void deserialize_internal_state(const nlohmann::json& json);

    private:
        bool _load_wav_sample(const std::filesystem::path& sample_path);
        void _initialize(abstract_configuration_directory& parent_config);

        const static_chunk_type _chunk_type;
        std::optional<std::filesystem::path> _sample_path{};
        unsigned int _sample_channel_id{0u};
        std::unique_ptr<abstract_configuration_page> _config_page{};
    };
}