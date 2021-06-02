
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
            main_gui& gui,
            circuit_tree& parent,
            unsigned int input_count,
            unsigned int output_count)
        : plugin_node_widget{"name", composite_node_plugin::uid, 
            [this, input_count, output_count]()
            {
                auto node = std::make_unique<DSPJIT::composite_node>(input_count, output_count);
                _composite_node = node.get();
                return std::move(node);
            }()}
        {
            _initialize(gui, parent, _default_name());
            _internal_editor->insert_node_widget(5, 5, _make_input_node());
            _internal_editor->insert_node_widget(10, 10, _make_output_node());
        }

        composite_node_widget(
            main_gui& gui,
            circuit_tree& parent,
            const nlohmann::json& state)
        : plugin_node_widget{"name", composite_node_plugin::uid, 
            [&state, this]()
            {
                const auto input_count = state.at("input-count").get<unsigned int>();
                const auto output_count = state.at("output-count").get<unsigned int>();
                auto node = std::make_unique<DSPJIT::composite_node>(input_count, output_count);
                _composite_node = node.get();
                return std::move(node);
            }()}
        {
            _initialize(gui, parent, state.at("name").get<std::string>());

            _internal_editor->deserialize(
                state.at("internal-circuit"),
                [this] (const nlohmann::json& j) -> std::unique_ptr<node_widget>
                {
                    if (j.is_string())
                        return _deserialize_internal_node(j.get<std::string>());
                    else
                        return _gui->create_node(*_editor_dir, j);
                });
        }

        ~composite_node_widget()
        {
            _gui->remove_config(*_editor_dir);
        }

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

        void _initialize(main_gui& gui, circuit_tree& parent, const std::string& node_name)
        {
            _gui = &gui;
            
            //  Create circuit editor
            auto editor = std::make_unique<circuit_editor>(100, 100);
            _internal_editor = editor.get();

            editor->set_circuit_changed_callback(
                [this]()
                {
                    _gui->compile();
                });

            editor->set_create_node_callback(
                [this]()
                {
                    return _gui->create_node(*_editor_dir);
                });    

            // Create composite node toolbox
            auto toolbox = std::make_unique<View::panel<>>(100, 100);

            auto name_text_input = std::make_unique<View::text_input>();
            name_text_input->set_text("NewName");

            auto name_button = std::make_unique<View::text_push_button>("set name");
            name_button->set_callback(
                [this, input = name_text_input.get()]()
                {
                    const auto& new_name = input->get_text();

                    try {
                        // Rename the configuration page
                        _gui->rename_config(*_editor_dir, new_name);

                        // Rename the node widget name
                        set_name(new_name);
                    }
                    catch (std::exception&)
                    {
                        LOG_WARNING("[composite node][config toolbox] Cannot rename configuration\n");
                    }
                });
            
            auto export_button = std::make_unique<View::text_push_button>("export");
            export_button->set_callback(
                [this]
                {
                    _gui->register_user_node(serialize(), name());
                });

            toolbox->insert_widget(5, 5, std::move(name_text_input));
            toolbox->insert_widget(5, 5 + 21 + 5, std::move(name_button));
            toolbox->insert_widget(5, 5 + 21 + 5 + 21 + 5, std::move(export_button));

            _editor_widget = 
                std::make_shared<View::map_wrapper>(
                    View::make_vertical_layout(
                        std::move(toolbox),
                        std::move(editor)
                    ), 
                    100, 100);

            //  Create editor dir with available name
            std::string new_name = node_name;
            auto& editor_dir = parent.insert_config_dir(new_name, circuit_tree{ _editor_widget });
            _editor_dir = &editor_dir;

            set_name(new_name);
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


        DSPJIT::composite_node *_composite_node;
        circuit_tree* _editor_dir{};
        circuit_editor* _internal_editor{};
        main_gui* _gui;
        std::shared_ptr<View::widget> _editor_widget{};
    };

    // Composite node plugin implementation

    composite_node_plugin::composite_node_plugin(main_gui& gui)
    :   node_widget_factory::plugin{
            composite_node_plugin::uid,
            "Circuit",
            "Composite"
        },
        _main_gui{gui}
    {
    }      

    std::unique_ptr<plugin_node_widget> composite_node_plugin::create_node(circuit_tree& dir)
    {
        return std::make_unique<composite_node_widget>(_main_gui, dir, 2, 2);
    }

    std::unique_ptr<plugin_node_widget> composite_node_plugin::create_node(circuit_tree& dir, const nlohmann::json& internal_state)
    {
        LOG_DEBUG("composite_node_plugin::create_node(const nlohmann::json& internal_state)\n");
        return std::make_unique<composite_node_widget>(_main_gui, dir, internal_state);
    }

}