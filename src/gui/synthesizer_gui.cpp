
#include "synthesizer_gui.h"
#include "internal_node_widget.h"

namespace Gammou {

    std::unique_ptr<node_widget> synthesizer_gui::make_master_from_polyphonic_node(synthesizer& synth)
    {
        return std::make_unique<internal_node_widget>(
            "From Polyphonic",
            _master_from_polyphonic_node_id,
            synth.from_polyphonic_node());
    }

    std::unique_ptr<node_widget> synthesizer_gui::make_master_output_node(synthesizer& synth)
    {
        return std::make_unique<internal_node_widget>(
            "Output",
            _master_output_node_id,
            synth.output_node());
    }

    std::unique_ptr<node_widget> synthesizer_gui::make_polyphonic_midi_input_node(synthesizer& synth)
    {
        auto midi_input =
            std::make_unique<internal_node_widget>(
            "Midi In",
            _polyphonic_midi_input_node_id,
            synth.midi_input_node());

        midi_input->set_output_name(0u, "Gate");
        midi_input->set_output_name(1u, "Pitch");
        midi_input->set_output_name(2u, "Attack");
        midi_input->set_output_name(3u, "Release");

        return midi_input;
    }

    std::unique_ptr<node_widget> synthesizer_gui::make_polyphonic_to_master_node(synthesizer& synth)
    {
        return std::make_unique<internal_node_widget>(
            "To Master",
            _polyphonic_to_master_node_id,
            synth.to_master_node());
    }

    std::unique_ptr<node_widget> synthesizer_gui::deserialize_internal_node(const std::string& identifier, synthesizer& synth)
    {
        if (identifier == _master_from_polyphonic_node_id)
            return make_master_from_polyphonic_node(synth);
        else if (identifier == _master_output_node_id)
            return make_master_output_node(synth);
        else if (identifier ==  _polyphonic_midi_input_node_id)
            return make_polyphonic_midi_input_node(synth);
        else  if (identifier == _polyphonic_to_master_node_id)
            return make_polyphonic_to_master_node(synth);
        else
            throw std::runtime_error("main_gui::deserialize : Unknown internal node : " + identifier);
    }

    std::unique_ptr<circuit_editor> synthesizer_gui::make_editor(synthesizer::circuit_controller& circuit)
    {
        auto editor = std::make_unique<circuit_editor>(200, 400);
        editor->set_circuit_changed_callback([&circuit]() { circuit.compile(); } );
        return editor;
    }

}