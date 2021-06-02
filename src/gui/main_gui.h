#ifndef GAMMOU_MAIN_GUI_H_
#define GAMMOU_MAIN_GUI_H_

#include <functional>

#include "view.h"

#include "synthesizer/synthesizer.h"
#include "plugin_system/node_widget_factory.h"
#include "factory_widget.h"

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

        View::widget& widget() noexcept { return *_main_widget; }


        std::unique_ptr<node_widget> create_node(circuit_tree& parent_config, const nlohmann::json& state);
        std::unique_ptr<node_widget> create_node(circuit_tree& parent_config, node_widget_factory::plugin_id id);
        std::unique_ptr<node_widget> create_node(circuit_tree& parent_config);
        
        circuit_tree& rename_config(circuit_tree& config_dir, const std::string& new_name);
        void remove_config(circuit_tree& config_dir);
        
        void compile();

        void register_user_node(nlohmann::json&& state, const std::string& name);

        void deserialize(const nlohmann::json&);
        nlohmann::json serialize();

    private:
        /*
         *  circuit tree management
         */
        void _reset_content();

        /*
         * gui widgets helpers
         */
        std::unique_ptr<View::widget> _make_main_editor_widget();
        void _make_master_circuit_editor();
        void _make_polyphonic_circuit_editor();
        std::unique_ptr<View::widget> _make_plugin_browser();
        std::unique_ptr<View::widget> _make_circuit_browser();
        std::unique_ptr<View::widget> _make_patch_browser();
        std::unique_ptr<node_widget> _make_master_from_polyphonic_node();
        std::unique_ptr<node_widget> _make_master_output_node();
        std::unique_ptr<node_widget> _make_polyphonic_midi_input_node();
        std::unique_ptr<node_widget> _make_polyphonic_to_master_node();
        std::unique_ptr<View::widget> _make_toolbox(std::unique_ptr<View::widget>&& additional_toolbox);

        /*
         *  serialize/deserialize helper
         */
        std::unique_ptr<node_widget> _deserialize_node(circuit_tree& dir, const nlohmann::json&);
        std::unique_ptr<node_widget> _deserialize_internal_node(const std::string& identifier);

        circuit_tree _circuit_tree{};
        circuit_tree *_master_circuit_dir{nullptr};
        circuit_tree *_polyphonic_circuit_dir{nullptr};

        synthesizer& _synthesizer;
        node_widget_factory& _factory;

        std::shared_ptr<View::widget> _polyphonic_circuit_widget{};
        std::shared_ptr<View::widget> _master_circuit_widget{};
        circuit_editor *_polyphonic_circuit_editor{};
        circuit_editor *_master_circuit_editor{};

        View::widget_proxy<> *_main_editor{nullptr};
        factory_widget* _factory_widget{};
        std::function<void()> _update_circuit_browser_widget{};
        std::unique_ptr<View::widget> _main_widget{};
    };

}

#endif