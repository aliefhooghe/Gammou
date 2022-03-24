
#include <DSPJIT/log.h>

#include "configuration_widget.h"
#include "synthesizer_gui.h"
#include "helpers/layout_builder.h"

namespace Gammou
{
    class configuration_page final : public abstract_configuration_page
    {
    public:
        configuration_page(configuration_widget& config_widget, configuration_leaf& leaf);
        configuration_page(configuration_page&&) noexcept = delete;
        ~configuration_page() override;

        void compile() override;
        void register_static_memory_chunk(const DSPJIT::compile_node_class& node, std::vector<uint8_t>&& data) override;
        void free_static_memory_chunk(const DSPJIT::compile_node_class& node) override;
        void display() override;
        void rename(const std::string& name) override;

    private:
        configuration_widget& _config_widget;
        configuration_leaf* _leaf{};
    };

    class configuration_directory final : public abstract_configuration_directory
    {
    public:
        configuration_directory(configuration_widget& config_widget, configuration_tree& directory);
        configuration_directory(configuration_directory&&) noexcept = delete;
        configuration_directory(const configuration_directory&) = delete;
        ~configuration_directory() override;

        void compile() override;
        void register_static_memory_chunk(const DSPJIT::compile_node_class& node, std::vector<uint8_t>&& data) override;
        void free_static_memory_chunk(const DSPJIT::compile_node_class& node) override;
        void display() override;
        void rename(const std::string& name) override;

        std::unique_ptr<abstract_configuration_directory> create_directory(
            std::string& desired_name, std::shared_ptr<View::widget>&& widget) override;

        std::unique_ptr<abstract_configuration_page> create_page(
            std::string &desired_name, std::shared_ptr<View::widget>&& widget) override;

    private:
        configuration_widget& _config_widget;
        configuration_tree* _dir{};
    };

    /**
     *  Configuration directory implementation
     */
    configuration_directory::configuration_directory(configuration_widget& config_widget, configuration_tree& directory)
    :   _config_widget{config_widget}, _dir{&directory}
    {
    }

    configuration_directory::~configuration_directory()
    {
        _config_widget.data_model().remove_config(*_dir);
        _config_widget.update();
    }

    void configuration_directory::compile()
    {
        if (auto ctl = _dir->get_circuit_controller())
            ctl->compile();
    }

    void configuration_directory::register_static_memory_chunk(const DSPJIT::compile_node_class& node, std::vector<uint8_t>&& data)
    {
        if (auto ctl = _dir->get_circuit_controller())
            ctl->register_static_memory_chunk(node, std::move(data));
    }

    void configuration_directory::free_static_memory_chunk(const DSPJIT::compile_node_class& node)
    {
        if (auto ctl = _dir->get_circuit_controller())
            ctl->free_static_memory_chunk(node);
    }

    void configuration_directory::display()
    {
        _config_widget._select_config(*_dir);
    }

    void configuration_directory::rename(const std::string& name)
    {
        auto& new_dir = _config_widget.data_model().rename_config(*_dir, name);
        _dir = &new_dir;
        _config_widget.update();
    }

    std::unique_ptr<abstract_configuration_directory> configuration_directory::create_directory(
        std::string &desired_name, std::shared_ptr<View::widget>&& widget)
    {
        auto new_dir = std::make_unique<configuration_directory>(
            _config_widget,
            _dir->insert_config_dir(desired_name, configuration_tree{std::move(widget)}));
        _config_widget.update();
        return new_dir;
    }

    std::unique_ptr<abstract_configuration_page> configuration_directory::create_page(
        std::string &desired_name, std::shared_ptr<View::widget>&& widget)
    {
        auto new_page = std::make_unique<configuration_page>(
            _config_widget,
            _dir->insert_config_leaf(desired_name, configuration_leaf{std::move(widget)}));
        _config_widget.update();
        return new_page;
    }

    /**
     *  Configuration page implementation
     */
     configuration_page::configuration_page(configuration_widget& config_widget, configuration_leaf& leaf)
    :  _config_widget{config_widget},
       _leaf{&leaf}
    {
    }

    configuration_page::~configuration_page()
    {
        _config_widget.data_model().remove_config(*_leaf);
        _config_widget.update();
    }

    void configuration_page::compile()
    {
        if (auto ctl = _leaf->get_circuit_controller())
            ctl->compile();
    }

    void configuration_page::register_static_memory_chunk(const DSPJIT::compile_node_class& node, std::vector<uint8_t>&& data)
    {
        if (auto ctl = _leaf->get_circuit_controller())
            ctl->register_static_memory_chunk(node, std::move(data));
    }

    void configuration_page::free_static_memory_chunk(const DSPJIT::compile_node_class& node)
    {
        if (auto ctl = _leaf->get_circuit_controller())
            ctl->free_static_memory_chunk(node);
    }

    void configuration_page::display()
    {
       _config_widget._select_config(*_leaf);
    }

    void configuration_page::rename(const std::string& name)
    {
        auto& new_leaf = _config_widget.data_model().rename_config(*_leaf, name);
        _leaf = &new_leaf;
        _config_widget.update();
    }


    /**
     *  Synthesizer state serialization/deserialization
     */
    struct synthesizer_state
    {
        nlohmann::json master_circuit{};
        nlohmann::json polyphonic_circuit{};
        synthesizer::voice_mode voicing_mode{synthesizer::voice_mode::POLYPHONIC};
    };

    NLOHMANN_JSON_SERIALIZE_ENUM(synthesizer::voice_mode, {
        {synthesizer::voice_mode::POLYPHONIC, "polyphonic"},
        {synthesizer::voice_mode::LEGATO, "legato"}
    })
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(synthesizer_state, master_circuit, polyphonic_circuit, voicing_mode)

    /**
     *  Configuration widget implementation
     */
    configuration_widget::configuration_widget(
        factory_widget& factory,
        synthesizer& synth,
        View::widget_proxy<>& editor_proxy,
        float width, float height)
    :   View::owning_directory_view<configuration_tree>(
            std::make_unique<configuration_tree>(),
            width, height,
            16.f,/* cell height */
            14.f /* font size */),
        _factory{factory},
        _synthesizer{synth},
        _editor_proxy{editor_proxy}
    {
        _initialize();

        // Initialize config tree view callbacks
        set_value_select_callback(
            [this](configuration_leaf& leaf)
            {
                // select and display the node configuration
                _editor_proxy.set_widget(leaf.get_config_widget());
            });

        set_directory_select_callback(
            [this](configuration_tree& dir)
            {
                // select and display the circuit
                _editor_proxy.set_widget(dir.get_config_widget());
            });
    }

    void configuration_widget::deserialize_configuration(const nlohmann::json& json)
    {
        try {
            synthesizer_state state{};
            from_json(json, state);

            reset_editor();

            _master_circuit_editor->deserialize(
                state.master_circuit,
                [this](const nlohmann::json &j)
                {
                    return _deserialize_node(*_master_circuit_dir, j);
                });
            _polyphonic_circuit_editor->deserialize(
                state.polyphonic_circuit,
                [this](const nlohmann::json &j)
                {
                    return _deserialize_node(*_polyphonic_circuit_dir, j);
                });

            _synthesizer.set_voice_mode(state.voicing_mode);

            // Recompile the new loaded circuit
            _synthesizer.get_master_circuit_controller().compile();
            _synthesizer.get_polyphonic_circuit_controller().compile();
        }
        catch (const std::exception &e) {
            reset_editor();
            throw;
        }
    }

    nlohmann::json configuration_widget::serialize_configuration()
    {
        const synthesizer_state state{
            _master_circuit_editor->serialize(),
            _polyphonic_circuit_editor->serialize(),
            _synthesizer.get_voice_mode()
        };
        nlohmann::json json{};
        to_json(json, state);
        return json;
    }

    void configuration_widget::_select_config(configuration_tree& config_dir)
    {
        _editor_proxy.set_widget(config_dir.get_config_widget());
        select_directory(config_dir);
    }

    void configuration_widget::_select_config(configuration_leaf& config_leaf)
    {
        _editor_proxy.set_widget(config_leaf.get_config_widget());
        select_value(config_leaf);
    }

    void configuration_widget::reset_editor()
    {
        LOG_DEBUG("[configuration_widget] Reset content\n");

        // Remove the nodes : this will also remove relevant configuration dirs
        _master_circuit_editor->clear();
        _master_circuit_editor->insert_node_widget(
            50, 50, synthesizer_gui::make_master_from_polyphonic_node(_synthesizer));
        _master_circuit_editor->insert_node_widget(
            50, 100, synthesizer_gui::make_master_output_node(_synthesizer));

        _polyphonic_circuit_editor->clear();
        _polyphonic_circuit_editor->insert_node_widget(
            50, 50, synthesizer_gui::make_polyphonic_midi_input_node(_synthesizer));
        _polyphonic_circuit_editor->insert_node_widget(
            50, 100, synthesizer_gui::make_polyphonic_to_master_node(_synthesizer));

        // Update the circuit browser
        update();

        //  Select master circuit by default
        _master_circuit_dir->display();

        // Recompile circuits
        _synthesizer.get_master_circuit_controller().compile();
        _synthesizer.get_polyphonic_circuit_controller().compile();
    }

    void configuration_widget::_initialize()
    {
        // initialize circuit editors
        auto master_editor = synthesizer_gui::make_editor(_synthesizer.get_master_circuit_controller());
        auto polyphonic_editor = synthesizer_gui::make_editor(_synthesizer.get_polyphonic_circuit_controller());

        _master_circuit_editor = master_editor.get();
        _polyphonic_circuit_editor = polyphonic_editor.get();

        auto master_circuit_widget = _wrap_editor(std::move(master_editor));
        auto polyphonic_circuit_widget = _wrap_editor(std::move(polyphonic_editor));


        std::string master_name = "Master";
        std::string polyphonic_name = "Polyphonic";

        auto& root = data_model();
        auto& master_circuit_dir =
            root.insert_config_dir(master_name, configuration_tree{std::move(master_circuit_widget), &_synthesizer.get_master_circuit_controller()});
        auto& polyphonic_circuit_dir =
            root.insert_config_dir(polyphonic_name, configuration_tree{std::move(polyphonic_circuit_widget), &_synthesizer.get_polyphonic_circuit_controller()});

        _master_circuit_dir = std::make_unique<configuration_directory>(*this, master_circuit_dir);
        _polyphonic_circuit_dir = std::make_unique<configuration_directory>(*this, polyphonic_circuit_dir);

        _master_circuit_editor->set_create_node_callback(
            [this]()
            {
                return _factory.create_node(*_master_circuit_dir);
            });
        _polyphonic_circuit_editor->set_create_node_callback(
            [this]()
            {
                return _factory.create_node(*_polyphonic_circuit_dir);
            });

        reset_editor();
    }

    std::shared_ptr<View::widget> configuration_widget::_wrap_editor(std::unique_ptr<circuit_editor>&& editor)
    {
        const View::layout_builder builder{};
        return builder.shared_header(builder.map(std::move(editor)), View::color_theme::color::SURFACE_DARK, 0.f /* no internal border */);
    }

    std::unique_ptr<node_widget> configuration_widget::_deserialize_node(abstract_configuration_directory& parent_config, const nlohmann::json& json)
    {
        if (json.is_string()) {
            return synthesizer_gui::deserialize_internal_node(json.get<std::string>(), _synthesizer);
        }
        else {
            return _factory.create_node(parent_config, json);
        }
    }
}