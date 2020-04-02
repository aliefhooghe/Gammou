#ifndef GAMMOU_MAIN_GUI_H_
#define GAMMOU_MAIN_GUI_H_

#include "view.h"
#include "synthesizer/synthesizer.h"
#include "plugin_system/node_widget_factory.h"

namespace Gammou {

    /**
     *  \brief Implement the logic of the main graphical user interface
     */
    class main_gui {

        static constexpr auto master_from_polyphonic_node_id = "from-polyphonic";
        static constexpr auto master_output_node_id = "output";
        static constexpr auto polyphonic_midi_input_node_id = "midi-input";
        static constexpr auto polyphonic_to_master_node_id = "to-master";

    public:
        main_gui(
            synthesizer& ,
            node_widget_factory& ,
            std::unique_ptr<View::widget>&& additional_toolbox = nullptr);

        View::widget& widget() noexcept { return *_widget; }

        void deserialize(const nlohmann::json&);
        nlohmann::json serialize();

    private:
        std::unique_ptr<circuit_editor> _make_master_circuit_editor();
        std::unique_ptr<circuit_editor> _make_polyphonic_circuit_editor();
        std::unique_ptr<View::widget> _make_toolbox(std::unique_ptr<View::widget>&& additional_toolbox);
        std::unique_ptr<View::widget> _make_left_sidebar();

        std::unique_ptr<node_widget> _make_master_from_polyphonic_node();
        std::unique_ptr<node_widget> _make_master_output_node();
        std::unique_ptr<node_widget> _make_polyphonic_midi_input_node();
        std::unique_ptr<node_widget> _make_polyphonic_to_master_node();

        circuit_editor *_polyphonic_circuit{nullptr};
        circuit_editor *_master_circuit{nullptr};
        synthesizer& _synthesizer;
        node_widget_factory& _factory;

        std::unique_ptr<View::widget> _widget{};
    };

}

#endif