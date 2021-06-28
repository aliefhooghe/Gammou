#ifndef GAMMOU_SYNTHESIZER_GUI_H
#define GAMMOU_SYNTHESIZER_GUI_H

#include "synthesizer/synthesizer.h"
#include "circuit_editor.h"

namespace Gammou {

    class synthesizer_gui
    {
    public:

        // Master nodes
        static std::unique_ptr<node_widget> make_master_from_polyphonic_node(synthesizer&);
        static std::unique_ptr<node_widget> make_master_output_node(synthesizer&);

        // Polyphonic circuit nodes
        static std::unique_ptr<node_widget> make_polyphonic_midi_input_node(synthesizer&);
        static std::unique_ptr<node_widget> make_polyphonic_to_master_node(synthesizer&);

        // Internal node deserialization
        static std::unique_ptr<node_widget> deserialize_internal_node(const std::string& identifier, synthesizer&);

        // Circuit editor building
        static std::unique_ptr<circuit_editor> make_editor(synthesizer::circuit_controller& circuit);

    private:
        static constexpr auto _master_from_polyphonic_node_id = "from-polyphonic";
        static constexpr auto _master_output_node_id = "output";
        static constexpr auto _polyphonic_midi_input_node_id = "midi-input";
        static constexpr auto _polyphonic_to_master_node_id = "to-master";


    };

}

#endif