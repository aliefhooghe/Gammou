#include "application.h"
#include "builtin_plugins/load_builtin_plugins.h"
#include "gui/composite_node/composite_node_plugin.h"
#include "gui/configuration_widget.h"
#include "gui/control_node_widgets/load_control_plugins.h"
#include "gui/factory_widget.h"
#include "helpers/layout_builder.h"
#include "patch_browser.h"
#include "plugin_system/package_loader.h"
#include "voice_mode_selector.h"

namespace Gammou
{

    application::application(
        const configuration& config,
        synthesizer& synth,
        std::unique_ptr<View::widget>&& additional_toolbox)
    {
        _factory = node_widget_factory_builder{synth.get_llvm_context()}
            .load_packages(config.packages_path)
            .build();

        // Load control and builtin plugins into factory
        load_control_plugins(synth, *_factory);
        load_builtin_plugins(*_factory);

        // Set up the main gui
        _main_gui = _make_main_gui(config, synth, std::move(additional_toolbox));

        //  Prepare synthesizer to use plugins
        synth.add_library_module(_factory->module());
    }

    nlohmann::json application::serialize()
    {
        return _configuration_widget->serialize_configuration();
    }

    void application::deserialize(const nlohmann::json& json)
    {
        _configuration_widget->deserialize_configuration(json);
        _voice_mode_selector->sync();
    }

    View::widget& application::main_gui() noexcept
    {
        return *_main_gui;
    }

    std::unique_ptr<View::widget> application::_make_main_gui(
        const configuration& config,
        synthesizer& synth,
        std::unique_ptr<View::widget>&& additional_toolbox)
    {
        auto main_editor_proxy = std::make_unique<View::widget_proxy<>>(1200, 720);
        auto factory_browser = std::make_unique<factory_widget>(*_factory, 100, 300);
        auto configuration_browser =
            std::make_unique<configuration_widget>(
                *factory_browser, synth, *main_editor_proxy, 210, 150);

        // keep a pointer on the configuration widget
        _configuration_widget = configuration_browser.get();

        // create the composite node plugin
        _factory->register_plugin(std::make_unique<composite_node_plugin>(*factory_browser));
        factory_browser->rescan_factory();

        //  Build the gui
        const View::layout_builder builder{};

        return builder.windows(
                builder.horizontal<false>(
                    builder.vertical<false>(
                        make_patch_browser(*this, config.patchs_path),
                        builder.header(std::move(factory_browser)),
                        builder.header(std::move(configuration_browser))),
                    builder.vertical(
                        _make_toolbox(synth, std::move(additional_toolbox)),
                        std::move(main_editor_proxy)
                )));
    }

    std::unique_ptr<View::widget> application::_make_toolbox(synthesizer& synth, std::unique_ptr<View::widget>&& additional_toolbox)
    {
        // Reset button
        auto reset_button = std::make_unique<View::text_push_button>("Reset");
        reset_button->set_callback([this]() { _configuration_widget->reset_editor(); });

        // Voice mode selector
        auto voice_mode_control = std::make_unique<voice_mode_selector>(synth);
        _voice_mode_selector = voice_mode_control.get();

        const View::layout_builder builder{};
        auto common_toolbox =
            builder.vertical(
                std::move(reset_button),
                std::move(voice_mode_control));

        if (!additional_toolbox)
            return builder.header(std::move(common_toolbox));
        else
            return builder.horizontal<false>(
                builder.header(std::move(common_toolbox)),
                std::move(additional_toolbox));
    }
}