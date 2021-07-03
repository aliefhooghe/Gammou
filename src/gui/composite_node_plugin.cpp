
#include <sstream>

#include "composite_node_plugin.h"
#include "composite_node.h"
#include "../gui/internal_node_widget.h"
#include "helpers/layout_builder.h"

#include "log.h"

namespace Gammou {

    struct composite_node_state
    {
        std::string name;
        unsigned int input_count;
        unsigned int output_count;
        nlohmann::json internal_circuit_state;
    };

    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(composite_node_state, name, input_count, output_count, internal_circuit_state)

    class composite_node_widget : public plugin_node_widget {
        static constexpr auto composite_input_id = "composite_input";
        static constexpr auto composite_output_id = "composite_output";
    public:
        composite_node_widget(
            factory_widget& factory,
            abstract_configuration_directory& parent_config,
            unsigned int input_count,
            unsigned int output_count)
        : plugin_node_widget{"name", composite_node_plugin::uid,
            [this, input_count, output_count]()
            {
                auto node = std::make_unique<DSPJIT::composite_node>(input_count, output_count);
                _composite_node = node.get();
                return std::move(node);
            }()},
          _factory{factory}
        {
            _initialize(parent_config, _default_name());
            _internal_editor->insert_node_widget(5, 5, _make_input_node());
            _internal_editor->insert_node_widget(10, 10, _make_output_node());
        }

        composite_node_widget(
            factory_widget& factory,
            abstract_configuration_directory& parent_config,
            const composite_node_state& state)
        : plugin_node_widget{"name", composite_node_plugin::uid,
            [&state, this]()
            {
                auto node = std::make_unique<DSPJIT::composite_node>(state.input_count, state.output_count);
                _composite_node = node.get();
                return std::move(node);
            }()},
          _factory{factory}
        {
            _initialize(parent_config, state.name);

            _internal_editor->deserialize(
                state.internal_circuit_state,
                [this] (const nlohmann::json& j) -> std::unique_ptr<node_widget>
                {
                    if (j.is_string())
                        return _deserialize_internal_node(j.get<std::string>());
                    else
                        return _factory.create_node(*_config_dir, j);
                });
        }

        ~composite_node_widget() noexcept = default;

        nlohmann::json serialize_internal_state() override
        {
            composite_node_state state{
                name(),
                node().get_input_count(),
                node().get_output_count(),
                _internal_editor->serialize()
            };

            nlohmann::json json;
            to_json(json, state);
            return json;
        }

    private:
        std::unique_ptr<node_widget>_make_input_node()
        {
            return std::make_unique<internal_node_widget>(
                    "Input", composite_input_id, _composite_node->input());
        }

        std::unique_ptr<node_widget> _make_output_node()
        {
            return std::make_unique<internal_node_widget>(
                    "Output", composite_output_id, _composite_node->output());
        }

        void _initialize(abstract_configuration_directory& parent_config, const std::string& node_name)
        {
            //  Create circuit editor
            auto editor = std::make_unique<circuit_editor>(100, 100);
            _internal_editor = editor.get();

            editor->set_circuit_changed_callback(
                [this]()
                {
                    _config_dir->compile();
                });

            editor->set_create_node_callback(
                [this]()
                {
                    return _factory.create_node(*_config_dir);
                });

            // Create composite node toolbox :

            // rename
            auto name_text_input = std::make_unique<View::text_input>();
            auto name_text_input_ptr = name_text_input.get();

            auto name_button = std::make_unique<View::text_push_button>("rename");
            name_button->set_callback(
                [this, input = name_text_input.get()]()
                {
                    const auto& new_name = input->get_text();

                    try {
                        // Rename the configuration page
                        _config_dir->rename(new_name);

                        // Rename the node widget name
                        set_name(new_name);
                    }
                    catch (std::exception&)
                    {
                        LOG_WARNING("[composite node][config toolbox] Cannot rename configuration\n");
                    }
                });

            // export
            auto export_button = std::make_unique<View::text_push_button>("export");
            export_button->set_callback(
                [this]
                {
                    _factory.register_user_node(*this, name());
                });

            View::layout_builder builder{};
            auto editor_widget =
                builder.shared_vertical(
                    builder.header(
                        builder.vertical(
                            builder.horizontal(std::move(name_text_input), std::move(name_button)),
                            std::move(export_button))
                    ),
                    builder.header(
                        builder.map(std::move(editor)),
                        View::color_theme::color::SURFACE_DARK,
                        0.f /* no internal border */));

            //  Create editor dir with available name
            std::string new_name = node_name;
            _config_dir = parent_config.create_directory(new_name, std::move(editor_widget));
            set_name(new_name);
            name_text_input_ptr->set_text(new_name);

            // Customize node widget : add edit button
            const auto edit_width = 30.f;
            const auto edit_height = 18.f;
            const auto edit_posx = (width() - edit_width) / 2.f;
            const auto edit_posy = height() - node_widget::node_header_size;
            auto edit_button = std::make_unique<View::text_push_button>("edit", 30, 18);
            edit_button->set_callback([this]() { _config_dir->display(); });
            insert_widget(edit_posx, edit_posy, std::move(edit_button));
        }

        std::unique_ptr<node_widget> _deserialize_internal_node(const std::string& identifier)
        {
            LOG_DEBUG("composite_node_widget _deserialize_internal_node %s\n", identifier.c_str());

            if (identifier == composite_input_id)
                return _make_input_node();
            else if (identifier == composite_output_id)
                return _make_output_node();
            else
                throw std::runtime_error("composite_node_plugin::deserialize : Unknown internal node : " + identifier);
        }

        std::string _default_name()
        {
            const auto ptr_val = reinterpret_cast<intptr_t>(this);
            std::stringstream ss;
            ss << std::hex << (ptr_val & 0xFFFF);
            return ss.str();
        }

        factory_widget& _factory;
        DSPJIT::composite_node *_composite_node;
        std::unique_ptr<abstract_configuration_directory> _config_dir{};
        circuit_editor *_internal_editor{};
    };

    // Composite node plugin implementation

    composite_node_plugin::composite_node_plugin(factory_widget& factory)
    :   node_widget_factory::plugin{
            composite_node_plugin::uid,
            "Circuit",
            "Composite"},
        _factory{factory}
    {
    }

    std::unique_ptr<plugin_node_widget> composite_node_plugin::create_node(abstract_configuration_directory& parent_config)
    {
        return std::make_unique<composite_node_widget>(_factory, parent_config, 2, 2);
    }

    std::unique_ptr<plugin_node_widget> composite_node_plugin::create_node(abstract_configuration_directory& parent_config, const nlohmann::json& json)
    {
        composite_node_state state;
        from_json(json, state);
        return std::make_unique<composite_node_widget>(_factory, parent_config, state);
    }
}