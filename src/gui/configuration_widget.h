#ifndef CONFIGURATION_WIDGET_H_
#define CONFIGURATION_WIDGET_H_

#include "configuration_tree.h"
#include "factory_widget.h"

namespace Gammou
{
    class configuration_widget : public View::owning_directory_view<configuration_tree>
    {
        friend class configuration_directory;
        friend class configuration_page;

        static constexpr auto master_from_polyphonic_node_id = "from-polyphonic";
        static constexpr auto master_output_node_id = "output";
        static constexpr auto polyphonic_midi_input_node_id = "midi-input";
        static constexpr auto polyphonic_to_master_node_id = "to-master";

    public:
        configuration_widget(
            factory_widget& factory,
            synthesizer& synth,
            View::widget_proxy<>& editor_proxy,
            float width, float height);

        bool deserialize_configuration(const nlohmann::json&);
        nlohmann::json serialize_configuration();

        void reset_editor();

    private:
        void _select_config(configuration_tree& config_dir);
        void _initialize();
        void _initialize_circuit_editors();

        std::unique_ptr<circuit_editor> _make_editor(synthesizer::circuit_controller& circuit);
        std::shared_ptr<View::widget> _wrap_editor(std::unique_ptr<circuit_editor>&& editor);
        std::unique_ptr<node_widget> _make_master_from_polyphonic_node();
        std::unique_ptr<node_widget> _make_master_output_node();
        std::unique_ptr<node_widget> _make_polyphonic_midi_input_node();
        std::unique_ptr<node_widget> _make_polyphonic_to_master_node();

        std::unique_ptr<node_widget> _deserialize_node(abstract_configuration_directory& parent_config, const nlohmann::json&);
        std::unique_ptr<node_widget> _deserialize_internal_node(const std::string& identifier);

        std::shared_ptr<View::widget> _polyphonic_circuit_widget{};
        std::shared_ptr<View::widget> _master_circuit_widget{};
        std::unique_ptr<abstract_configuration_directory> _master_circuit_dir{};
        std::unique_ptr<abstract_configuration_directory> _polyphonic_circuit_dir{};

        circuit_editor *_polyphonic_circuit_editor{};
        circuit_editor *_master_circuit_editor{};

        factory_widget& _factory;
        synthesizer& _synthesizer;
        View::widget_proxy<>& _editor_proxy;
    };
}

#endif /* CONFIGURATION_WIDGET_H_ */
