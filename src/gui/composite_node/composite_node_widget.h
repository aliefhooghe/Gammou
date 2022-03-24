#ifndef GAMMOU_COMPOSITE_NODE_WIDGET_H_
#define GAMMOU_COMPOSITE_NODE_WIDGET_H_

#include <DSPJIT/composite_node.h>
#include "gui/factory_widget.h"

namespace Gammou
{
    class io_naming_toolbox;

    class composite_node_widget : public plugin_node_widget
    {
        static constexpr auto composite_input_id = "composite_input";
        static constexpr auto composite_output_id = "composite_output";

    public:
        struct state
        {
            std::string name;
            std::vector<std::string> input_names;
            std::vector<std::string> output_names;
            nlohmann::json internal_circuit_state;
        };

        /**
         * \brief Connstruct a fresh new composite node
         */
        composite_node_widget(
            factory_widget &factory,
            abstract_configuration_directory &parent_config,
            unsigned int input_count,
            unsigned int output_count);

        /**
         * \brief Connstruct a composite node from saved state
         */
        composite_node_widget(
            factory_widget &factory,
            abstract_configuration_directory &parent_config,
            const state &state);

        ~composite_node_widget() noexcept = default;

        /**
         * \brief Serialize te internal circuit
         */
        nlohmann::json serialize_internal_state() override;

        /**
         * \brief Override nod_widget method to keep name text input
         * up to date
         */
        void set_name(const std::string& name) override;

        /**
         * \brief Rename input and apply the name to the internal input node
         */
        void set_input_name(unsigned int input_id, const std::string& name) override;

        /**
         * \brief Rename output and apply the name to the internal output node
         */
        void set_output_name(unsigned int output_id, const std::string& name) override;

    private:
        std::unique_ptr<node_widget> _make_input_node();
        std::unique_ptr<node_widget> _make_output_node();

        std::unique_ptr<circuit_editor> _initialize_internal_editor();
        std::unique_ptr<View::widget> _initialize_main_toolbox();
        std::unique_ptr<View::widget> _initialize_io_resize_toolbox(io_naming_toolbox&);

        void _initialize(abstract_configuration_directory &parent_config, const std::string &node_name);
        std::unique_ptr<node_widget> _deserialize_internal_node(const std::string &identifier);

        static std::string _default_name(const composite_node_widget *pointer);

        std::unique_ptr<DSPJIT::composite_node> _create_composite_node(
            unsigned int input_count, unsigned int output_count);

        factory_widget &_factory;
        DSPJIT::composite_node *_composite_node;
        std::unique_ptr<abstract_configuration_directory> _config_dir{};
        node_widget *_input_node{nullptr};
        node_widget *_output_node{nullptr};
        circuit_editor *_internal_editor{nullptr};
        View::text_input *_name_text_input{nullptr};
    };

    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(composite_node_widget::state, name, input_names, output_names, internal_circuit_state)
}

#endif