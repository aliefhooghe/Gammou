#include <sstream>

#include "composite_node_plugin.h"
#include "composite_node_widget.h"
#include "io_naming_toolbox.h"

#include "gui/internal_node_widget.h"

#include <DSPJIT/log.h>

#include "helpers/layout_builder.h"

namespace Gammou
{

    composite_node_widget::composite_node_widget(
        factory_widget &factory,
        abstract_configuration_directory &parent_config,
        unsigned int input_count,
        unsigned int output_count)
    : plugin_node_widget{
        "name", composite_node_plugin::uid,
        _create_composite_node(
            input_count,
            output_count)
     },
     _factory{factory}
    {
        _initialize(parent_config, _default_name(this));
        _internal_editor->insert_node_widget(5, 5, _make_input_node());
        _internal_editor->insert_node_widget(10, 10, _make_output_node());
    }

    composite_node_widget::composite_node_widget(
        factory_widget &factory,
        abstract_configuration_directory &parent_config,
        const composite_node_widget::state &state)
    : plugin_node_widget{
        "name", composite_node_plugin::uid,
        _create_composite_node(
            state.input_names.size(),
            state.output_names.size())
     },
     _factory{factory}
    {
        // Restore IO names from state
        for (auto i = 0; i < state.input_names.size(); ++i)
            set_input_name(i, state.input_names[i]);
        for (auto i = 0; i < state.output_names.size(); ++i)
            set_output_name(i, state.output_names[i]);

        _initialize(parent_config, state.name);

        _internal_editor->deserialize(
            state.internal_circuit_state,
            [this](const nlohmann::json &j) -> std::unique_ptr<node_widget>
            {
                if (j.is_string())
                    return _deserialize_internal_node(j.get<std::string>());
                else
                    return _factory.create_node(*_config_dir, j);
            });
    }

    nlohmann::json composite_node_widget::serialize_internal_state()
    {
        composite_node_widget::state state{
            name(),
            std::vector<std::string>{get_input_count()},
            std::vector<std::string>{get_output_count()},
            _internal_editor->serialize()};

        for (auto i = 0; i < state.input_names.size(); ++i)
            state.input_names[i] = get_input_name(i);
        for (auto i = 0; i < state.output_names.size(); ++i)
            state.output_names[i] = get_output_name(i);

        nlohmann::json json;
        to_json(json, state);
        return json;
    }

    void composite_node_widget::set_name(const std::string& name)
    {
        plugin_node_widget::set_name(name);
        if (_name_text_input != nullptr)
            _name_text_input->set_text(name);
    }

    void composite_node_widget::set_input_name(unsigned int input_id, const std::string& name)
    {
        plugin_node_widget::set_input_name(input_id, name);
        if (_input_node != nullptr)
            _input_node->set_output_name(input_id, name);
    }

    void composite_node_widget::set_output_name(unsigned int output_id, const std::string& name)
    {
        plugin_node_widget::set_output_name(output_id, name);
        if (_output_node != nullptr)
            _output_node->set_input_name(output_id, name);
    }

    std::unique_ptr<node_widget> composite_node_widget::_make_input_node()
    {
        auto node = std::make_unique<internal_node_widget>(
            "Input", composite_input_id, _composite_node->input());
        for (auto i = 0u; i < get_input_count(); ++i)
            node->set_output_name(i, get_input_name(i));
        _input_node = node.get();
        return node;
    }

    std::unique_ptr<node_widget> composite_node_widget::_make_output_node()
    {
        auto node = std::make_unique<internal_node_widget>(
            "Output", composite_output_id, _composite_node->output());
        for (auto i = 0u; i < get_output_count(); ++i)
            node->set_input_name(i, get_output_name(i));
        _output_node = node.get();
        return node;
    }

    std::unique_ptr<circuit_editor> composite_node_widget::_initialize_internal_editor()
    {

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

        return editor;
    }

    std::unique_ptr<View::widget> composite_node_widget::_initialize_main_toolbox()
    {
        // rename
        auto name_text_input = std::make_unique<View::text_input>();
        _name_text_input = name_text_input.get();

        auto name_button = std::make_unique<View::text_push_button>("rename circuit");
        name_button->set_callback(
            [this, input = name_text_input.get()]()
            {
                const auto &new_name = input->get_text();

                try
                {
                    // Rename the configuration page
                    _config_dir->rename(new_name);

                    // Rename the node widget
                    plugin_node_widget::set_name(new_name);
                }
                catch (std::exception &)
                {
                    input->set_text(name());
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

        // Create layout
        View::layout_builder builder{};
        return builder.header(
            builder.vertical(
                builder.horizontal(std::move(name_text_input), std::move(name_button)),
                std::move(export_button)));
    }

    std::unique_ptr<View::widget> composite_node_widget::_initialize_io_resize_toolbox(io_naming_toolbox& toolbox)
    {
        auto add_input_button = std::make_unique<View::text_push_button>("Input+");
        auto rem_input_button = std::make_unique<View::text_push_button>("Input-");
        auto add_output_button = std::make_unique<View::text_push_button>("Output+");
        auto rem_output_button = std::make_unique<View::text_push_button>("Output-");

        add_input_button->set_callback(
            [this, &toolbox]()
            {
                _composite_node->add_input();
                toolbox.update();
            });
        rem_input_button->set_callback(
            [this, &toolbox]()
            {
                _composite_node->remove_input();
                toolbox.update();
            });
        add_output_button->set_callback(
            [this, &toolbox]()
            {
                _composite_node->add_output();
                toolbox.update();
            });
        rem_output_button->set_callback(
            [this, &toolbox]()
            {
                _composite_node->remove_output();
                toolbox.update();
            });

        View::layout_builder builder{};
        return builder.vertical(
            builder.horizontal(std::move(add_input_button), std::move(add_output_button)),
            builder.horizontal(std::move(rem_input_button), std::move(rem_output_button)));
    }

    void composite_node_widget::_initialize(abstract_configuration_directory &parent_config, const std::string &node_name)
    {
        auto editor = _initialize_internal_editor();
        auto main_toolbox = _initialize_main_toolbox();
        auto io_naming = std::make_unique<io_naming_toolbox>(*this, 256.f);
        auto io_resize_toolbox = _initialize_io_resize_toolbox(*io_naming);

        // Create the main widget
        View::layout_builder builder{};
        auto editor_widget =
            builder.shared_horizontal(
                builder.vertical(
                    builder.header(std::move(io_naming)),
                    builder.header(std::move(io_resize_toolbox))
                ),
                builder.vertical(
                    std::move(main_toolbox),
                    builder.header(
                        builder.map(std::move(editor)),
                        View::color_theme::color::SURFACE_DARK,
                        0.f /* no internal border */)));

        //  Create editor dir with available name
        std::string new_name = node_name;
        _config_dir = parent_config.create_directory(new_name, std::move(editor_widget));
        set_name(new_name);

        // Customize node widget : add edit button
        // const auto edit_width = 30.f;
        // const auto edit_height = 18.f;
        // const auto edit_posx = (width() - edit_width) / 2.f;
        // const auto edit_posy = height() - node_widget::node_header_size;
        // auto edit_button = std::make_unique<View::text_push_button>("edit", 30, 18);
        // edit_button->set_callback([this]()
        //                           { _config_dir->display(); });
        // insert_widget(edit_posx, edit_posy, std::move(edit_button));
    }

    std::unique_ptr<node_widget> composite_node_widget::_deserialize_internal_node(const std::string &identifier)
    {
        LOG_DEBUG("composite_node_widget _deserialize_internal_node %s\n", identifier.c_str());

        if (identifier == composite_input_id)
            return _make_input_node();
        else if (identifier == composite_output_id)
            return _make_output_node();
        else
            throw std::runtime_error("composite_node_plugin::deserialize : Unknown internal node : " + identifier);
    }

    std::string composite_node_widget::_default_name(const composite_node_widget *pointer)
    {
        const auto ptr_val = reinterpret_cast<intptr_t>(pointer);
        std::stringstream ss;
        ss << std::hex << (ptr_val & 0xFFFF);
        return ss.str();
    }

    std::unique_ptr<DSPJIT::composite_node> composite_node_widget::_create_composite_node(
        unsigned int input_count,
        unsigned int output_count)
    {
        auto node =
            std::make_unique<DSPJIT::composite_node>(
                input_count,
                output_count);
        _composite_node = node.get();
        return std::move(node);
    }
}