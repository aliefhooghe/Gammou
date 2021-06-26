
#include <sstream>

#include "composite_node_plugin.h"
#include "composite_node.h"
#include "../gui/internal_node_widget.h"

#include "log.h"

namespace Gammou {

    constexpr auto composite_input_id = "composite-input";
    constexpr auto composite_output_id = "composite-output";

    class composite_node_widget : public plugin_node_widget {

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
            const nlohmann::json& state)
        : plugin_node_widget{"name", composite_node_plugin::uid,
            [&state, this]()
            {
                const auto input_count = state.at("input-count").get<unsigned int>();
                const auto output_count = state.at("output-count").get<unsigned int>();
                auto node = std::make_unique<DSPJIT::composite_node>(input_count, output_count);
                _composite_node = node.get();
                return std::move(node);
            }()},
          _factory{factory}
        {
            _initialize(parent_config, state.at("name").get<std::string>());

            _internal_editor->deserialize(
                state.at("internal-circuit"),
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
            return  {
                {"input-count", node().get_input_count()},
                {"output-count", node().get_output_count()},
                {"internal-circuit", _internal_editor->serialize()},
                {"name", name()}
            };
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
            auto toolbox = std::make_unique<View::panel<>>(100, 100);

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

            toolbox->insert_widget(5, 5, std::move(name_text_input));
            toolbox->insert_widget(5, 5 + 21 + 5, std::move(name_button));
            toolbox->insert_widget(5, 5 + 21 + 5 + 21 + 5, std::move(export_button));

            _editor_widget =
                View::make_shared_vertical_layout(
                    std::make_unique<View::header>(std::move(toolbox)),
                    std::make_unique<View::header>(
                        std::make_unique<View::map_wrapper>(
                            std::move(editor),
                            100, 100),
                        View::color_theme::color::SURFACE_DARK));

            //  Create editor dir with available name
            std::string new_name = node_name;
            _config_dir = parent_config.create_directory(new_name, _editor_widget);
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
        std::shared_ptr<View::widget> _editor_widget{};
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

    std::unique_ptr<plugin_node_widget> composite_node_plugin::create_node(abstract_configuration_directory& parent_config, const nlohmann::json& internal_state)
    {
        LOG_DEBUG("composite_node_plugin::create_node(const nlohmann::json& internal_state)\n");
        return std::make_unique<composite_node_widget>(_factory, parent_config, internal_state);
    }

}