
#include "helpers/alphabetical_compare.h"
#include "helpers/filesystem_directory_model.h"
#include "backends/common/configuration.h"
#include "circuit_editor.h"
#include "internal_node_widget.h"
#include "composite_node_plugin.h"

#include "main_gui.h"

#include <iomanip>
#include <fstream>
#include <sstream>
#include <thread>

namespace Gammou {

    static auto ptr_2_string(const void* ptr)
    {
        std::stringstream ss;
        ss << ptr;
        return  ss.str();
    }

    main_gui::main_gui(
        synthesizer& synthesizer,
        node_widget_factory& factory,
        std::unique_ptr<View::widget>&& additional_toolbox)
    :   _synthesizer{synthesizer},
        _factory{factory}
    {
        // Initialize synthesizer circuits editors
        _make_master_circuit_editor();
        _make_polyphonic_circuit_editor();

        // main circuit editor
        auto editor = _make_main_editor_widget();

        //  Register composite node plugin into factory
        _factory.register_plugin(std::make_unique<composite_node_plugin>(*this));

        //  left side bar
        auto left_sidebar =
            View::make_vertical_layout(
                _make_patch_browser(),
                _make_plugin_browser(),
                _make_circuit_browser()
            );

        //  toolbox
        auto toolbox = _make_toolbox(std::move(additional_toolbox));

        //  main Windows
        auto map_editor =
            std::make_unique<View::header>(
                std::make_unique<View::map_wrapper>(std::move(editor), 1300, 400),
                View::color_theme::color::SURFACE_DARK);

        _main_widget = std::make_unique<View::background>(
            View::make_horizontal_layout(
                left_sidebar,
                View::make_vertical_layout(
                    std::move(toolbox),
                    std::move(map_editor)
                )
            ));

        //
        _reset_circuit_tree();
    }


    std::unique_ptr<node_widget> main_gui::create_node(
        circuit_tree_model& dir,
        node_widget_factory::plugin_id id)
    {
        auto node = _factory.create_node(dir, id);
        _update_circuit_browser_widget();
        return node;
    }

    std::unique_ptr<node_widget> main_gui::create_node(circuit_tree_model& dir)
    {
        //  No plugin is being used
        if (_insert_plugin_id == node_widget_factory::no_id) {
            LOG_DEBUG("main_editor::_create_node : No plugin is being used\n");
            return nullptr;
        }

        return create_node(dir, _insert_plugin_id);
    }

    std::unique_ptr<node_widget> main_gui::create_node(circuit_tree_model& dir, const nlohmann::json& state)
    {
        auto node = _factory.create_node(dir, state);
        _update_circuit_browser_widget();
        return node;
    }

    void main_gui::compile()
    {
        _synthesizer.compile_master_circuit();
        _synthesizer.compile_polyphonic_circuit();
    }

    void main_gui::deserialize(const nlohmann::json& json)
    {
        _reset_circuit_tree();
        _master_circuit_editor->deserialize(
            json.at("master-circuit"),
            [this](const nlohmann::json& j)
            {
                return _deserialize_node(*_master_circuit_dir, j);
            });
        _polyphonic_circuit_editor->deserialize(
            json.at("polyphonic-circuit"),
            [this, ed = _polyphonic_circuit_editor.get()](const nlohmann::json& j)
            {
                return _deserialize_node(*_polyphonic_circuit_dir, j);
            });
    }

    nlohmann::json main_gui::serialize()
    {
        return {
            {"master-circuit", _master_circuit_editor->serialize()},
            {"polyphonic-circuit", _polyphonic_circuit_editor->serialize()}
        };
    }

    circuit_tree_model& main_gui::_add_root_circuit(
        const std::string& name,
        std::shared_ptr<circuit_editor> root_circuit)
    {
        auto& editor_dir = _circuit_tree.add_directory(name, circuit_tree_model{root_circuit});

        root_circuit->set_circuit_changed_callback(
            [this]()
            {
                compile();
            });

        root_circuit->set_create_node_callback(
            [this, &editor_dir]()
            {
                return create_node(editor_dir);
            });

        return editor_dir;
    }

    void main_gui::_reset_circuit_tree()
    {
        _circuit_tree.clear();
        auto& master_dir = _add_root_circuit("Master", _master_circuit_editor);
        auto& poly_dir = _add_root_circuit("Polyphonic", _polyphonic_circuit_editor);
        _master_circuit_dir = &master_dir;
        _polyphonic_circuit_dir = &poly_dir;
        _update_circuit_browser_widget();
    }

    std::unique_ptr<View::widget> main_gui::_make_main_editor_widget()
    {
        auto main_editor_widget = std::make_unique<View::widget_proxy<>>(200, 400);
        _main_editor = main_editor_widget.get();
        return main_editor_widget;
    }

    /**
     *  @brief build the synthesizer mastetr circuit editor widget
     */
    void main_gui::_make_master_circuit_editor()
    {
        _master_circuit_editor = std::make_shared<circuit_editor>(200, 400);

        _master_circuit_editor->insert_node_widget(50, 50, _make_master_from_polyphonic_node());
        _master_circuit_editor->insert_node_widget(50, 100, _make_master_output_node());

        _master_circuit_editor->set_circuit_changed_callback(
            [this](){ _synthesizer.compile_master_circuit(); });
    }

    /**
     *  @brief build the synthesizer polyphonic circuit editor widget
     */
    void main_gui::_make_polyphonic_circuit_editor()
    {
        _polyphonic_circuit_editor = std::make_shared<circuit_editor>(400, 200);

        _polyphonic_circuit_editor->insert_node_widget(50, 50, _make_polyphonic_midi_input_node());
        _polyphonic_circuit_editor->insert_node_widget(50, 100, _make_polyphonic_to_master_node());

        _polyphonic_circuit_editor->set_circuit_changed_callback(
            [this](){ _synthesizer.compile_polyphonic_circuit(); });
    }

    /**
     * @brief build the toolbox widget, integrating custom additional widget if any
     */
    std::unique_ptr<View::widget> main_gui::_make_toolbox(std::unique_ptr<View::widget>&& additional_toolbox)
    {
        auto common_toolbox = std::make_unique<View::panel<>>(250, 110);

        if (!additional_toolbox)
            return std::make_unique<View::header>(std::move(common_toolbox));
        else
            return View::make_horizontal_layout(std::make_unique<View::header>(std::move(common_toolbox)), std::move(additional_toolbox));
    }

    std::unique_ptr<View::widget> main_gui::_make_plugin_browser()
    {
        using node_class_model =
            View::storage_directory_model<std::string, node_widget_factory::plugin_id, View::alphabetical_compare>;

        auto node_classes = std::make_unique<node_class_model>();

        //  Insert each plugin
        for (const auto& pair : _factory) {
            auto uid = pair.first;
            const auto& plugin = pair.second;

            //  Create or get existing category directory
            auto& category_dir = node_classes->add_directory(plugin->category());

            //  Insert plugin in category directory
            category_dir.add_value(plugin->name(), std::move(uid));
        }

        auto node_class_selector = View::make_directory_view(std::move(node_classes), 100, 500);
        node_class_selector->set_value_select_callback(
            [this](auto uid)
            {
                _insert_plugin_id = uid;
            });

        return std::make_unique<View::header>(std::move(node_class_selector));
    }

    std::unique_ptr<View::widget> main_gui::_make_circuit_browser()
    {
        auto circuit_browser = View::make_directory_view(_circuit_tree, 100, 500);

        circuit_browser->set_value_select_callback(
            [this](std::weak_ptr<View::widget> config)
            {
                // select and display the node configuration
                _main_editor->set_widget(config);
            });

        circuit_browser->set_directory_select_callback(
            [this](circuit_tree_model& dir)
            {
                // select and display the circuit
                _main_editor->set_widget(dir.editor);
            });

        _update_circuit_browser_widget =
            [br = circuit_browser.get()]()
            {
                br->update();
            };

        return std::make_unique<View::header>(std::move(circuit_browser));
    }

    std::unique_ptr<View::widget> main_gui::_make_patch_browser()
    {
        const auto patch_dir_path = configuration::get_patch_path();

        auto preset_name_input = std::make_unique<View::text_input>(110, 21);
        auto save_button = std::make_unique<View::text_push_button>("Save", 80, 21);
        auto update_button = std::make_unique<View::text_push_button>("Update", 195, 21);

        auto filesystem_view =
            View::make_directory_view(
                std::make_unique<View::filesystem_directory_model>(patch_dir_path),
                140, 90);

        update_button->set_callback(
            [fv = filesystem_view.get()]()
            {
                fv->data_model().sync();
                fv->update();
            });

        filesystem_view->set_value_select_callback(
            [this](const auto& preset_path)
            {
                LOG_INFO("[main gui] Loading patch '%s'\n", preset_path.c_str());
                try {
                    nlohmann::json json;
                    std::ifstream stream{preset_path, std::ios_base::in};

                    if (!stream.good())
                        throw std::exception{};

                    stream >> json;
                    deserialize(json);
                }
                catch(const std::exception& e)
                {
                    LOG_ERROR("[main gui] Failed to load preset : %s\n", e.what());
                }
                catch(...)
                {
                    LOG_ERROR("[main gui] Failed to load preset : unknown error\n");
                }
            });

        save_button->set_callback(
            [this, input = preset_name_input.get(), fv = filesystem_view.get(), patch_dir_path]()
            {
                try {
                    const auto preset_path = patch_dir_path / input->get_text();
                    std::ofstream stream{preset_path, std::ios_base::trunc};

                    if (!stream.good())
                        throw std::exception{};

                    const auto json = serialize();
                    stream << std::setw(4) << json;
                    stream.close();
                    fv->data_model().sync();
                    fv->update();
                    LOG_INFO("[main gui] Saved patch '%s'\n", preset_path.c_str());
                }
                catch(const std::exception& e)
                {
                    LOG_ERROR("[main gui] Failed to save preset : %s\n", e.what());
                }
                catch(...)
                {
                    LOG_ERROR("[main gui] Failed to save preset : unknown error\n");
                }
            });

        auto panel = std::make_unique<View::panel<>>(210, 57);
        panel->insert_widget(5, 5, std::move(preset_name_input));
        panel->insert_widget(5 + 110 + 5, 5, std::move(save_button));
        panel->insert_widget(5, 5 + 21 + 5, std::move(update_button));

        return std::make_unique<View::header>(
            View::make_vertical_layout(
                std::move(panel),
                std::move(filesystem_view)
            ));
    }

    std::unique_ptr<node_widget> main_gui::_make_master_from_polyphonic_node()
    {
        return std::make_unique<internal_node_widget>("From Polyphonic", master_from_polyphonic_node_id, _synthesizer.from_polyphonic_node());
    }

    std::unique_ptr<node_widget> main_gui::_make_master_output_node()
    {
        return std::make_unique<internal_node_widget>("Output", master_output_node_id, _synthesizer.output_node());
    }

    std::unique_ptr<node_widget> main_gui::_make_polyphonic_midi_input_node()
    {
        auto midi_input_node = std::make_unique<internal_node_widget>("Midi Input", polyphonic_midi_input_node_id, _synthesizer.midi_input_node());

        midi_input_node->set_output_name(0u, "Gate");
        midi_input_node->set_output_name(1u, "Pitch");
        midi_input_node->set_output_name(2u, "Attack");

        return midi_input_node;
    }

    std::unique_ptr<node_widget> main_gui::_make_polyphonic_to_master_node()
    {
        return std::make_unique<internal_node_widget>("To Master", polyphonic_to_master_node_id, _synthesizer.to_master_node());
    }

    std::unique_ptr<node_widget> main_gui::_deserialize_node(circuit_tree_model& dir, const nlohmann::json& j)
    {
        return j.is_string() ?
            _deserialize_internal_node(j.get<std::string>()) :
            create_node(dir, j);
    }

    std::unique_ptr<node_widget> main_gui::_deserialize_internal_node(const std::string& identifier)
    {
        if (identifier == master_from_polyphonic_node_id)
            return _make_master_from_polyphonic_node();
        else if (identifier == master_output_node_id)
            return _make_master_output_node();
        else if (identifier ==  polyphonic_midi_input_node_id)
            return _make_polyphonic_midi_input_node();
        else  if (identifier == polyphonic_to_master_node_id)
            return _make_polyphonic_to_master_node();
        else
            throw std::runtime_error("main_gui::deserialize : Unknown internal node : " + identifier);
    }
}