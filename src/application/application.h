#ifndef GAMMOU_APPLICATION_H_
#define GAMMOU_APPLICATION_H_

#include "gui/configuration_widget.h"
#include "voice_mode_selector.h"

namespace Gammou {

    class application
    {
    public:

        struct configuration
        {
            std::filesystem::path packages_path{};
            std::filesystem::path patchs_path{};
        };

        application(
            const configuration& config,
            synthesizer& synth,
            std::unique_ptr<View::widget>&& additional_toolbox = {});

        /**
         * \brief Serialize application state to json object
         */
        nlohmann::json serialize();

        /**
         * \brief Deserialize application state from json object
         */
        void deserialize(const nlohmann::json& json);

        /**
         * \brief Get a reference on the gui main widget
         */
        View::widget& main_gui() noexcept;

    private:
        std::unique_ptr<View::widget> _make_main_gui(
            const configuration& config,
            synthesizer& synth,
            std::unique_ptr<View::widget>&& additional_toolbox);
        std::unique_ptr<View::widget> _make_toolbox(
            synthesizer& synth,
            std::unique_ptr<View::widget>&& additional_toolbox);

        std::unique_ptr<node_widget_factory> _factory{};
        configuration_widget *_configuration_widget{nullptr};
        voice_mode_selector *_voice_mode_selector{nullptr};
        std::unique_ptr<View::widget> _main_gui{};
    };

}

#endif /* */