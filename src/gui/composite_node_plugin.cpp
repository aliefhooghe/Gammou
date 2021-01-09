
#include <sstream>

#include "composite_node_plugin.h"
#include "composite_node.h"
#include "../gui/internal_node_widget.h"

#include "log.h"

namespace Gammou {

    static auto ptr_2_string(const void* ptr)
    {
        std::stringstream ss;
        ss << ptr;
        return  ss.str();
    }

    constexpr auto composite_input_id = "composite-input";
    constexpr auto composite_output_id = "composite-output";

    class composite_node_widget : public plugin_node_widget {

    public:
        composite_node_widget(
            main_gui& gui,
            circuit_tree_model& dir,
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
            _initialize_internal_editor(gui, dir);
            _internal_editor->insert_node_widget(5, 5, _make_input_node());
            _internal_editor->insert_node_widget(10, 10, _make_output_node());
        }

        composite_node_widget(
            main_gui& gui,
            circuit_tree_model& dir,
            const nlohmann::json& internal_circuit_state)
        : plugin_node_widget{"name", composite_node_plugin::uid, 
            [&internal_circuit_state, this]()   
            {
                const auto input_count = internal_circuit_state.at("input-count").get<unsigned int>();
                const auto output_count = internal_circuit_state.at("output-count").get<unsigned int>();
                auto node = std::make_unique<DSPJIT::composite_node>(input_count, output_count);
                _composite_node = node.get();
                return std::move(node);
            }()}
        {
            auto& editor_dir = _initialize_internal_editor(gui, dir);

            _internal_editor->deserialize(
                internal_circuit_state.at("internal-circuit"),
                [this, &gui, &editor_dir] (const nlohmann::json& j) -> std::unique_ptr<node_widget>
                {
                    if (j.is_string())
                        return _deserialize_internal_node(j.get<std::string>());
                    else
                        return gui.create_node(editor_dir, j);
                });
        }

        nlohmann::json serialize_internal_state() override
        {
            return  {
                {"input-count", node().get_input_count()},
                {"output-count", node().get_output_count()},
                {"internal-circuit", _internal_editor->serialize()}
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

        circuit_tree_model& _initialize_internal_editor(main_gui& gui, circuit_tree_model& dir)
        {
            _internal_editor = std::make_shared<circuit_editor>(100, 100);
            
            auto& editor_dir = 
                dir.add_directory("composite-" + ptr_2_string(this), circuit_tree_model{_internal_editor});

            _internal_editor->set_circuit_changed_callback(
                [&gui]()
                {
                    gui.compile();
                });

            _internal_editor->set_create_node_callback(
                [&gui, &editor_dir]()
                {
                    return gui.create_node(editor_dir);
                });

            return editor_dir;            
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

        DSPJIT::composite_node *_composite_node;
        std::shared_ptr<circuit_editor> _internal_editor{};
    };

    composite_node_plugin::composite_node_plugin(main_gui& gui)
    :   node_widget_factory::plugin{
            composite_node_plugin::uid,
            "Circuit",
            "Composite"
        },
        _main_gui{gui}
    {
    }      

    std::unique_ptr<plugin_node_widget> composite_node_plugin::create_node(circuit_tree_model& dir)
    {
        return std::make_unique<composite_node_widget>(_main_gui, dir, 2, 2);
    }

    std::unique_ptr<plugin_node_widget> composite_node_plugin::create_node(circuit_tree_model& dir, const nlohmann::json& internal_state)
    {
        LOG_DEBUG("composite_node_plugin::create_node(const nlohmann::json& internal_state)\n");
        return std::make_unique<composite_node_widget>(_main_gui, dir, internal_state);
    }

}