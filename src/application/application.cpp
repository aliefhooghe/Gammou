#include "application.h"
#include "gui/factory_widget.h"
#include "gui/configuration_widget.h"
#include "gui/composite_node_plugin.h"

#include "patch_browser.h"

namespace Gammou
{

    application::application(
        synthesizer& synth,
        node_widget_factory& factory,
        std::unique_ptr<View::widget>&& additional_toolbox)
    {
        _main_gui = _make_main_gui(synth, factory, std::move(additional_toolbox));
    }

    nlohmann::json application::serialize()
    {
        return _configuration_widget->serialize_configuration();
    }

    bool application::deserialize(nlohmann::json& json)
    {
        return _configuration_widget->deserialize_configuration(json);
    }

    View::widget& application::main_gui() noexcept
    {
        return *_main_gui;
    }

    std::unique_ptr<View::widget> application::_make_main_gui(
        synthesizer& synth,
        node_widget_factory& factory,
        std::unique_ptr<View::widget>&& additional_toolbox)
    {
        auto main_editor_proxy = std::make_unique<View::widget_proxy<>>(1200, 750);
        auto factory_browser = std::make_unique<factory_widget>(factory, 100, 500);
        auto configuration_browser =
            std::make_unique<configuration_widget>(
                *factory_browser, synth, *main_editor_proxy, 210, 60);

        // keep a pointer on the configuration widget
        _configuration_widget = configuration_browser.get();

        // create the composite node plugin
        factory.register_plugin(std::make_unique<composite_node_plugin>(*factory_browser));
        factory_browser->rescan_factory();

        //  left side bar
        auto left_sidebar =
            View::make_vertical_layout(
                make_patch_browser(*this),
                std::make_unique<View::header>(std::move(factory_browser)),
                std::make_unique<View::header>(std::move(configuration_browser)));

        auto toolbox = _make_toolbox(std::move(additional_toolbox));

        return std::make_unique<View::background>(
                    View::make_horizontal_layout(
                        std::move(left_sidebar),
                        View::make_vertical_layout(
                            std::move(toolbox),
                            std::move(main_editor_proxy))));
    }

    std::unique_ptr<View::widget> application::_make_toolbox(std::unique_ptr<View::widget>&& additional_toolbox)
    {
        const auto common_toolbox_width = additional_toolbox ? 100 : 700;
        auto common_toolbox = std::make_unique<View::panel<>>(common_toolbox_width, 110);

        auto reset_button = std::make_unique<View::text_push_button>("Reset", 80, 21);
        reset_button->set_callback([this]() { _configuration_widget->reset_editor(); });

        common_toolbox->insert_widget(5, 5, std::move(reset_button));

        if (!additional_toolbox)
            return std::make_unique<View::header>(std::move(common_toolbox));
        else
            return View::make_horizontal_layout(std::make_unique<View::header>(std::move(common_toolbox)), std::move(additional_toolbox));
    }
}