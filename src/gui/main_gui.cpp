
#include "circuit_editor.h"
#include "main_gui.h"


namespace Gammou {

    std::unique_ptr<View::widget> make_synthesizer_gui(synthesizer& synthesizer, std::unique_ptr<View::widget>&& additional_toolbox)
    {
        //  master circuit editor
        auto master_circuit_editor = std::make_unique<circuit_editor>(20, 20);

        master_circuit_editor->insert_node_widget(10, 10,
            std::make_unique<node_widget>("From Polyphonic", synthesizer.from_polyphonic_node()));

        master_circuit_editor->insert_node_widget(10, 20,
            std::make_unique<node_widget>("Output", synthesizer.output_node()));

        master_circuit_editor->set_circuit_changed_callback(
            [&synthesizer](){ synthesizer.compile_master_circuit(); });

        //  polyphonic circuit editor
        auto polyphonic_circuit_editor = std::make_unique<circuit_editor>(20, 20);

        polyphonic_circuit_editor->insert_node_widget(10, 10,
            std::make_unique<node_widget>("Midi Input", synthesizer.midi_input_node()));

        polyphonic_circuit_editor->insert_node_widget(10, 20,
            std::make_unique<node_widget>("To Master", synthesizer.to_master_node()));

        polyphonic_circuit_editor->set_circuit_changed_callback(
            [&synthesizer](){ synthesizer.compile_polyphonic_circuit(); });

        auto left_side_bar = std::make_unique<View::header>(std::make_unique<View::panel<>>(30, 100));
        auto common_toolbox = std::make_unique<View::header>(std::make_unique<View::panel<>>(30, 30));
        std::unique_ptr<View::widget> toolbox{};

        if (!additional_toolbox)
            toolbox = std::move(common_toolbox);
        else
            toolbox = View::make_horizontal_layout(additional_toolbox, std::move(common_toolbox));


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