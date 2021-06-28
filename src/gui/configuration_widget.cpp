
#include "configuration_widget.h"
#include "synthesizer_gui.h"

namespace Gammou
{
    class configuration_page final : public abstract_configuration_page
    {
    public:
        configuration_page(configuration_widget& config_widget, configuration_leaf& leaf);
        configuration_page(configuration_page&&) noexcept = delete;
        ~configuration_page() override;

        void compile();
        void register_static_memory_chunk(const DSPJIT::compile_node_class& node, std::vector<uint8_t>&& data);
        void free_static_memory_chunk(const DSPJIT::compile_node_class& node);
        void display();
        void rename(const std::string& name);

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
            std::string& desired_name, std::weak_ptr<View::widget> widget) override;

        std::unique_ptr<abstract_configuration_page> create_page(
            std::string &desired_name, std::weak_ptr<View::widget> widget) override;

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
        std::string &desired_name, std::weak_ptr<View::widget> widget)
    {
        auto new_dir = std::make_unique<configuration_directory>(
            _config_widget,
            _dir->insert_config_dir(desired_name, configuration_tree{widget}));
        _config_widget.update();
        return new_dir;
    }

     std::unique_ptr<abstract_configuration_page> configuration_directory::create_page(
         std::string &desired_name, std::weak_ptr<View::widget> widget)
     {
         auto new_page = std::make_unique<configuration_page>(
             _config_widget,
             _dir->insert_config_leaf(desired_name, configuration_leaf{widget}));
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
         LOG_ERROR("[configuration_page] Display is not implemented\n");
     }

     void configuration_page::rename(const std::string& name)
     {
         auto& new_leaf = _config_widget.data_model().rename_config(*_leaf, name);
         _leaf = &new_leaf;
         _config_widget.update();
     }

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

    bool configuration_widget::deserialize_configuration(const nlohmann::json& json)
    {
        try {
            reset_editor();
            _master_circuit_editor->deserialize(
                json.at("master-circuit"),
                [this](const nlohmann::json &j)
                {
                    return _deserialize_node(*_master_circuit_dir, j);
                });
            _polyphonic_circuit_editor->deserialize(
                json.at("polyphonic-circuit"),
                [this](const nlohmann::json &j)
                {
                    return _deserialize_node(*_polyphonic_circuit_dir, j);
                });

            // Recompile the new loaded circuit
            _synthesizer.get_master_circuit_controller().compile();
            _synthesizer.get_polyphonic_circuit_controller().compile();
            return true;
        }
        catch (const std::exception &e) {
            reset_editor();
            LOG_ERROR("[configuration_widget Failed to load preset : %s\n", e.what());
            return false;
        }
        catch (...) {
            reset_editor();
            LOG_ERROR("[configuration_widget] Failed to load preset : unknown error\n");
            return false;
        }
    }

    nlohmann::json configuration_widget::serialize_configuration()
    {
        return {
            {"master-circuit", _master_circuit_editor->serialize()},
            {"polyphonic-circuit", _polyphonic_circuit_editor->serialize()}
        };
    }

    void configuration_widget::_select_config(configuration_tree& config_dir)
    {
        _editor_proxy.set_widget(config_dir.get_config_widget());
        select_directory(config_dir);
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
        _editor_proxy.set_widget(_master_circuit_widget);

        // Recompile circuits
        _synthesizer.get_master_circuit_controller().compile();
        _synthesizer.get_polyphonic_circuit_controller().compile();
    }

    void configuration_widget::_initialize()
    {
        _initialize_circuit_editors();

        std::string master_name = "Master";
        std::string polyphonic_name = "Polyphonic";

        auto& root = data_model();
        auto& master_circuit_dir =
            root.insert_config_dir(master_name, configuration_tree{_master_circuit_widget, &_synthesizer.get_master_circuit_controller()});
        auto& polyphonic_circuit_dir =
            root.insert_config_dir(polyphonic_name, configuration_tree{_polyphonic_circuit_widget, &_synthesizer.get_polyphonic_circuit_controller()});

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

    void configuration_widget::_initialize_circuit_editors()
    {
        auto master_editor = synthesizer_gui::make_editor(_synthesizer.get_master_circuit_controller());
        auto polyphonic_editor = synthesizer_gui::make_editor(_synthesizer.get_polyphonic_circuit_controller());

        _master_circuit_editor = master_editor.get();
        _polyphonic_circuit_editor = polyphonic_editor.get();

        _master_circuit_widget = _wrap_editor(std::move(master_editor));
        _polyphonic_circuit_widget = _wrap_editor(std::move(polyphonic_editor));
    }

    std::shared_ptr<View::widget> configuration_widget::_wrap_editor(std::unique_ptr<circuit_editor>&& editor)
    {
        return std::make_shared<View::header>(
                    std::make_unique<View::map_wrapper>(
                        std::move(editor),
                        100, 100),
                    View::color_theme::color::SURFACE_DARK);
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