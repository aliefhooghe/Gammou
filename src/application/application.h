#ifndef GAMMOU_APPLICATION_H_
#define GAMMOU_APPLICATION_H_

#include "gui/configuration_widget.h"

namespace Gammou {

    class application
    {
    public:
        application(
            synthesizer& synth,
            std::unique_ptr<View::widget>&& additional_toolbox = {});

        /**
         * \brief Serialize application state to json object
         */
        nlohmann::json serialize();

        /**
         * \brief Deserialize application state from json object
         */
        bool deserialize(nlohmann::json& json);

        /**
         * \brief Get a reference on the gui main widget
         */
        View::widget& main_gui() noexcept;

    private:
        std::unique_ptr<View::widget> _make_main_gui(
            synthesizer& synth,
            std::unique_ptr<View::widget>&& additional_toolbox);
        std::unique_ptr<View::widget> _make_toolbox(
            synthesizer& synth,
            std::unique_ptr<View::widget>&& additional_toolbox);

        std::unique_ptr<node_widget_factory> _factory{};
        configuration_widget *_configuration_widget{nullptr};
        std::unique_ptr<View::widget> _main_gui{};
    };

}

#endif /* */