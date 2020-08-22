
#include "helpers/alphabetical_compare.h"
#include "helpers/filesystem_directory_model.h"
#include "circuit_editor.h"
#include "internal_node_widget.h"
#include "main_gui.h"

#include <iomanip>
#include <fstream>

namespace Gammou {

    main_gui::main_gui(
        synthesizer& synthesizer,
        node_widget_factory& factory,
        std::unique_ptr<View::widget>&& additional_toolbox)
    :   _synthesizer{synthesizer},
        _factory{factory}
    {
        //  circuit editors
        auto master_circuit_editor = _make_master_circuit_editor();
        auto polyphonic_circuit_editor = _make_polyphonic_circuit_editor();

        _polyphonic_circuit = polyphonic_circuit_editor.get();
        _master_circuit = master_circuit_editor.get();

        //  left side bar
        auto left_sidebar = _make_left_sidebar();

        //  toolbox
        auto toolbox = _make_toolbox(std::move(additional_toolbox));

        //  main Windows
        auto polyphonic_map_editor =
            std::make_unique<View::header>(std::make_unique<View::map_wrapper>(std::move(polyphonic_circuit_editor), 130, 40));
        auto master_map_editor =
            std::make_unique<View::header>(std::make_unique<View::map_wrapper>(std::move(master_circuit_editor), 130, 40));

        _widget = std::make_unique<View::background>(
            View::make_horizontal_layout(
                left_sidebar
                ,

                View::make_vertical_layout(
                    std::move(toolbox),
                    std::move(polyphonic_map_editor),
                    std::move(master_map_editor)
                )
            ));
    }

    void main_gui::deserialize(const nlohmann::json& json)
    {
        auto node_deserialize_callback =
            [this](const nlohmann::json& j) -> std::unique_ptr<node_widget>
            {
                if (j.is_string()) {
                    //  It is an internal node
                    const auto identifier = j.get<std::string>();

                    if (identifier == master_from_polyphonic_node_id)
                        return _make_master_from_polyphonic_node();
                    else if (identifier == master_output_node_id)
                        return _make_master_output_node();
                    else if (identifier ==  polyphonic_midi_input_node_id)
                        return _make_polyphonic_midi_input_node();
                    else  if (identifier == polyphonic_to_master_node_id)
                        return _make_polyphonic_to_master_node();
                    else {
                        std::cerr << "Bad identifier = " << identifier << std::endl;
                        throw std::runtime_error("main_gui::deserialize : Unknown internal node");
                    }
                }
                else {
                    return _factory.create_node(
                        j.at("plugin-uid").get<uint64_t>(),
                        j.at("state"));
                }
            };

        _master_circuit->deserialize(json.at("master-circuit"), node_deserialize_callback);
        _polyphonic_circuit->deserialize(json.at("polyphonic-circuit"), node_deserialize_callback);
    }

    nlohmann::json main_gui::serialize()
    {
        return {
            {"master-circuit", _master_circuit->serialize()},
            {"polyphonic-circuit", _polyphonic_circuit->serialize()}
        };
    }

    /**
     *  @brief build the synthesizer mastetr circuit editor widget
     */
    std::unique_ptr<circuit_editor> main_gui::_make_master_circuit_editor()
    {
        auto master_circuit_editor = std::make_unique<circuit_editor>(20, 40);

        master_circuit_editor->insert_node_widget(5, 5, _make_master_from_polyphonic_node());
        master_circuit_editor->insert_node_widget(5, 10, _make_master_output_node());

        master_circuit_editor->set_circuit_changed_callback(
            [this](){ _synthesizer.compile_master_circuit(); });

        return master_circuit_editor;
    }

    /**
     *  @brief build the synthesizer polyphonic circuit editor widget
     */
    std::unique_ptr<circuit_editor> main_gui::_make_polyphonic_circuit_editor()
    {
        auto polyphonic_circuit_editor = std::make_unique<circuit_editor>(40, 20);

        polyphonic_circuit_editor->insert_node_widget(5, 5, _make_polyphonic_midi_input_node());
        polyphonic_circuit_editor->insert_node_widget(5, 10, _make_polyphonic_to_master_node());

        polyphonic_circuit_editor->set_circuit_changed_callback(
            [this](){ _synthesizer.compile_polyphonic_circuit(); });

        return polyphonic_circuit_editor;
    }

    /**
     * @brief build the toolbox widget, integrating custom additional widget if any
     */
    std::unique_ptr<View::widget> main_gui::_make_toolbox(std::unique_ptr<View::widget>&& additional_toolbox)
    {
        auto common_toolbox = std::make_unique<View::panel<>>(10, 5);

        if (!additional_toolbox)
            return std::make_unique<View::header>(std::move(common_toolbox));
        else
            return View::make_horizontal_layout(std::make_unique<View::header>(std::move(common_toolbox)), std::move(additional_toolbox));
    }

    /**
     * @brief build the left sidebar
     */
    std::unique_ptr<View::widget> main_gui::_make_left_sidebar()
    {
        return View::make_vertical_layout(
            _make_preset_tool_box(),
            _make_node_selector()
        );
    }

    std::unique_ptr<View::widget> main_gui::_make_preset_tool_box()
    {
        const std::filesystem::path preset_dir_path{"/home/aliefhooghe/Documents/PRESET_DEBUG"};

        auto preset_name_input = std::make_unique<View::text_input>(10.f, 1.5f);
        auto save_button = std::make_unique<View::text_push_button>("Save", 5.f, 1.5f);
        auto update_button = std::make_unique<View::text_push_button>("Update", 15.5f);

        auto filesystem_view =
            View::make_directory_view(
                std::make_unique<View::filesystem_directory_model>(preset_dir_path),
                16.5f, 8.f);

        update_button->set_callback([fv = filesystem_view.get()]() { fv->update(); });

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
                catch(...)
                {
                    LOG_ERROR("[main gui] Failed to load preset\n");
                }
            });

        save_button->set_callback(
            [this, input = preset_name_input.get(), fv = filesystem_view.get(), preset_dir_path]()
            {
                try {
                    const auto preset_path = preset_dir_path / input->get_text();
                    std::ofstream stream{preset_path, std::ios_base::trunc};

                    if (!stream.good())
                        throw std::exception{};

                    const auto json = serialize();
                    stream << std::setw(4) << json;
                    stream.close();
                    fv->update();
                    LOG_INFO("[main gui] Saved patch '%s'\n", preset_path.c_str());
                }
                catch(...)
                {
                    LOG_ERROR("[main gui] Failed to save preset\n");
                }
            });

        auto panel = std::make_unique<View::panel<>>(16.5f, 4.5f);
        panel->insert_widget(0.5f, 0.5f, std::move(preset_name_input));
        panel->insert_widget(11.f, 0.5f, std::move(save_button));
        panel->insert_widget(0.5f, 2.5f, std::move(update_button));

        return std::make_unique<View::header>(
            View::make_vertical_layout(
                std::move(panel),
                std::move(filesystem_view)
            ));
    }

    std::unique_ptr<View::widget> main_gui::_make_node_selector()
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

        auto node_class_selector = View::make_directory_view(std::move(node_classes), 10, 50);
        node_class_selector->set_value_select_callback(
            [this](auto uid)
            {
                auto create_node_callback = [this, uid]() { return _factory.create_node(uid); };
                _master_circuit->set_create_node_callback(create_node_callback);
                _polyphonic_circuit->set_create_node_callback(create_node_callback);
            });

        return std::make_unique<View::header>(std::move(node_class_selector));
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


}