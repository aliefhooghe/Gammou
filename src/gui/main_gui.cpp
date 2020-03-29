

#include "helpers/alphabetical_compare.h"
#include "circuit_editor.h"

#include "main_gui.h"


namespace Gammou {

    /**
     *  @brief build the synthesizer mastetr circuit editor widget
     */
    static auto make_master_circuit_editor(synthesizer& synthesizer)
    {
        auto master_circuit_editor = std::make_unique<circuit_editor>(20, 20);

        master_circuit_editor->insert_node_widget(5, 5,
            std::make_unique<node_widget>("From Polyphonic", synthesizer.from_polyphonic_node()));

        master_circuit_editor->insert_node_widget(5, 10,
            std::make_unique<node_widget>("Output", synthesizer.output_node()));

        master_circuit_editor->set_circuit_changed_callback(
            [&synthesizer](){ synthesizer.compile_master_circuit(); });

        return master_circuit_editor;
    }

    /**
     *  @brief build the synthesizer polyphonic circuit editor widget
     */
    static auto make_polyphonic_circuit_editor(synthesizer& synthesizer)
    {
        auto polyphonic_circuit_editor = std::make_unique<circuit_editor>(20, 20);

        auto midi_input_node =
            std::make_unique<node_widget>("Midi Input", synthesizer.midi_input_node());
        midi_input_node->set_output_name(0u, "Gate");
        midi_input_node->set_output_name(1u, "Pitch");
        polyphonic_circuit_editor->insert_node_widget(5, 5, std::move(midi_input_node));

        polyphonic_circuit_editor->insert_node_widget(5, 10,
            std::make_unique<node_widget>("To Master", synthesizer.to_master_node()));

        polyphonic_circuit_editor->set_circuit_changed_callback(
            [&synthesizer](){ synthesizer.compile_polyphonic_circuit(); });

        return polyphonic_circuit_editor;
    }

    /**
     * @brief build the toolbox widget, integrating custom additional widget if any
     */
    static auto make_toolbox(std::unique_ptr<View::widget>&& additional_toolbox)
    {
        auto common_toolbox = std::make_unique<View::header>(std::make_unique<View::panel<>>(10, 5));
        std::unique_ptr<View::widget> toolbox{};

        if (!additional_toolbox)
            toolbox = std::move(common_toolbox);
        else
            toolbox = View::make_horizontal_layout(std::move(common_toolbox), std::move(additional_toolbox));

        return toolbox;
    }

    std::unique_ptr<View::widget> make_synthesizer_gui(
        synthesizer& synthesizer,
        node_widget_factory& factory,
        std::unique_ptr<View::widget>&& additional_toolbox)
    {
        //  circuit editors
        auto master_circuit_editor = make_master_circuit_editor(synthesizer);
        auto polyphonic_circuit_editor = make_polyphonic_circuit_editor(synthesizer);

        // master_circuit_editor->set_create_node_callback(
        //     [&factory]()
        //     {
        //         return factory.create_node(42u);
        //     }
        // );

        //  polyphonic circuit editor


        // polyphonic_circuit_editor->set_create_node_callback(
        //     [&factory]()
        //     {
        //         return factory.create_node(42u);
        //     }
        // );

        //  left side bar

        using node_class_model =
            View::storage_directory_model<std::string, node_widget_factory::uid, View::alphabetical_compare>;

        auto node_classes = std::make_unique<node_class_model>();

        //  Insert each plugin
        for (const auto& pair : factory) {
            auto uid = pair.first;
            const auto& plugin = pair.second;

            //  Create or get existing category directory
            auto& category_dir = node_classes->add_directory(plugin->get_category());

            //  Insert plugin in category directory
            category_dir.add_value(plugin->get_name(), std::move(uid));
        }

        auto node_class_selector = View::make_directory_view(std::move(node_classes), 10, 50);
        node_class_selector->set_value_select_callback(
            [&factory, master = master_circuit_editor.get(), polyphonic = polyphonic_circuit_editor.get()](auto uid)
            {
                auto create_node_callback = [&factory, uid]() { return factory.create_node(uid); };
                master->set_create_node_callback(create_node_callback);
                polyphonic->set_create_node_callback(create_node_callback);
            });

        auto left_side_bar = std::make_unique<View::header>(std::move(node_class_selector));

        //  toolbox
        std::unique_ptr<View::widget> toolbox = make_toolbox(std::move(additional_toolbox));

        //  main Windows
        auto polyphonic_map_editor = std::make_unique<View::header>(std::make_unique<View::map_wrapper>(std::move(polyphonic_circuit_editor), 20, 20));
        auto master_map_editor = std::make_unique<View::header>(std::make_unique<View::map_wrapper>(std::move(master_circuit_editor), 20, 20));

        return std::make_unique<View::background>(
            View::make_horizontal_layout(
                left_side_bar
                ,

                View::make_vertical_layout(
                    std::move(toolbox),
                    std::move(polyphonic_map_editor),
                    std::move(master_map_editor)
                )
            ));
    }
}