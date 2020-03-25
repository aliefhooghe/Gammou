
#include "circuit_editor.h"
#include "main_gui.h"


namespace Gammou {

    std::unique_ptr<View::widget> make_synthesizer_gui(synthesizer& synthesizer)
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

        return std::make_unique<View::background>(
            std::make_unique<View::pair_layout<View::orientation::vertical>>(
                std::make_unique<View::header>(std::move(polyphonic_circuit_editor)),
                std::make_unique<View::header>(std::move(master_circuit_editor))
            ));
    }

}