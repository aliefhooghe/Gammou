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

        std::shared_ptr<View::widget> _wrap_editor(std::unique_ptr<circuit_editor>&& editor);
        std::unique_ptr<node_widget> _deserialize_node(abstract_configuration_directory& parent_config, const nlohmann::json&);

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
