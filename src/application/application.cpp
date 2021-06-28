#include "application.h"
#include "backends/common/configuration.h"
#include "gui/factory_widget.h"
#include "gui/configuration_widget.h"
#include "gui/composite_node_plugin.h"
#include "plugin_system/package_loader.h"
#include "patch_browser.h"
#include "builtin_plugins/load_builtin_plugins.h"
#include "gui/control_node_widgets/load_control_plugins.h"

namespace Gammou
{

    application::application(
        synthesizer& synth,
        std::unique_ptr<View::widget>&& additional_toolbox)
    {
        _factory = node_widget_factory_builder{synth.get_llvm_context()}
            .load_packages(configuration::get_packages_directory_path())
            .build();

        // Load control and builtin plugins into factory
        load_control_plugins(synth, *_factory);
        load_builtin_plugins(*_factory);

        // Set up the main gui
        _main_gui = _make_main_gui(synth, std::move(additional_toolbox));

        //  Prepare synthesizer to use plugins
        synth.add_library_module(_factory->module());
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
        std::unique_ptr<View::widget>&& additional_toolbox)
    {
        auto main_editor_proxy = std::make_unique<View::widget_proxy<>>(1200, 750);
        auto factory_browser = std::make_unique<factory_widget>(*_factory, 100, 500);
        auto configuration_browser =
            std::make_unique<configuration_widget>(
                *factory_browser, synth, *main_editor_proxy, 210, 60);

        // keep a pointer on the configuration widget
        _configuration_widget = configuration_browser.get();

        // create the composite node plugin
        _factory->register_plugin(std::make_unique<composite_node_plugin>(*factory_browser));
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