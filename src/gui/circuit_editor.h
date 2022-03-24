#ifndef GAMMOU_CIRCUIT_EDITOR_H_
#define GAMMOU_CIRCUIT_EDITOR_H_

#include <functional>
#include <memory>
#include <unordered_map>
#include <vector>
#include <string>
#include <nlohmann/json.hpp>

#include <DSPJIT/compile_node_class.h>
#include <view.h>


namespace Gammou
{

    class circuit_editor;

    class node_widget : public View::panel_implementation<> {
        friend class circuit_editor;
    public:
        node_widget(const std::string& name, DSPJIT::compile_node_class&);
        ~node_widget() override = default;

        void apply_color_theme(const View::color_theme& theme) override;

        virtual void set_name(const std::string& name);
        virtual void set_input_name(unsigned int input_id, const std::string& name);
        virtual void set_output_name(unsigned int output_id, const std::string& name);

        void set_internal(bool internal = true) noexcept { _is_internal = internal; }
        bool is_internal() const noexcept { return _is_internal; }

        const std::string& name() const noexcept { return _name; }
        const std::string& get_input_name(const unsigned int input_id);
        const std::string& get_output_name(const unsigned int output_id);

        unsigned int get_input_count();
        unsigned int get_output_count();

        auto& node() const noexcept { return _node; }
        auto& node() noexcept { return _node; }

        void draw(NVGcontext* vg) override;
        void draw_rect(NVGcontext* vg, const View::rectangle<>&rect) override { draw(vg);} /* \todo */

        virtual nlohmann::json serialize() =0;

        //  1 unit = font size
        static constexpr auto node_header_size = 20;
        static constexpr auto node_corner_radius = 10;
        static constexpr auto socket_size = 13;
        static constexpr auto socket_hitbox_size = socket_size * 2;
        static constexpr auto socket_radius = 3;
        static constexpr auto node_width = 120;

    private:
        static auto _default_input_name(unsigned int idx) { return "in " + std::to_string(idx); }
        static auto _default_output_name(unsigned int idx) { return "out " + std::to_string(idx); }

        bool _input_id_at(unsigned int& input_id, float x, float y);
        bool _output_id_at(unsigned int& output_id, float x, float y);

        void _input_pos(unsigned int input_id, float& x, float& y);
        void _output_pos(unsigned int output_id, float& x, float& y);

        void _check_io_counts();

        DSPJIT::compile_node_class& _node;
        std::string _name;
        std::vector<std::string> _input_names;
        std::vector<std::string> _output_names;
        bool _is_internal{false}; // an internal node can not be removed by user

        //  colors
        NVGcolor _text_color;
        NVGcolor _border_color;
        NVGcolor _background_color;
        NVGcolor _socket_color;
    };

    class owning_node_widget : public node_widget {
    public:
        owning_node_widget(const std::string& name, std::unique_ptr<DSPJIT::compile_node_class>&& node)
        :   node_widget{name, *node},
            _node{std::move(node)}
        {}

    private:
        std::unique_ptr<DSPJIT::compile_node_class> _node;
    };

    class circuit_editor : public View::panel_implementation<node_widget> {
    public:

        /**
         *  \brief Called to create a node at dbl click event
         **/
        using create_node_callback =
            std::function<std::unique_ptr<node_widget>(void)>;

        /**
         *  \brief  Called when the circuit was changed :
         *          -   link added/removed
         *          -   component removed
         **/
        using circuit_changed_callback =
            std::function<void(void)>;

        using node_deserializer =
            std::function<std::unique_ptr<node_widget>(const nlohmann::json&)>;

        circuit_editor(float width, float height);
        circuit_editor(float width, float height, View::size_constraint width_constraint, View::size_constraint height_constraint);

        void insert_node_widget(float x, float y, std::unique_ptr<node_widget>&&);
        void remove_node_widget(node_widget*);
        void clear();

        bool on_mouse_move(float x, float y) override;
        bool on_mouse_drag(const View::mouse_button button, float x, float y, float dx, float dy) override;
        bool on_mouse_drag_start(const View::mouse_button button, float x, float y) override;
        bool on_mouse_drag_end(const View::mouse_button button, float x, float y) override;
        bool on_mouse_drag_cancel() override;
        bool on_mouse_button_up(const View::mouse_button button, float x, float y) override;

        void apply_color_theme(const View::color_theme& theme) override;

        /*
         *      Event callbacks
         **/
        void set_create_node_callback(create_node_callback);
        void set_circuit_changed_callback(circuit_changed_callback);

        /*
         *      Serialization / Deserialization
         */
        nlohmann::json serialize();
        void deserialize(const nlohmann::json&, node_deserializer);

        void draw(NVGcontext *vg) override;
        void draw_rect(NVGcontext *vg, const View::rectangle<> &rect) override { draw(vg); } /* \todo */

    protected:
        void _draw_link(
            NVGcontext *vg, NVGcolor color,
            float x_input, float y_input,
            float x_output, float y_output, float link_width) const noexcept;

        void _notify_circuit_change();

    private:
        create_node_callback _create_node_callback{};
        circuit_changed_callback _circuit_changed_callback{};

        std::unordered_map<const DSPJIT::compile_node_class *, node_widget *> _node_widgets{};

        //  mouse drag state machine
        enum class drag_state
        {
            OFF,
            DRAG_NODE,
            MOVE_NODE,
            LINK_NODE
        };

        drag_state _drag_state{drag_state::OFF};
        node_widget *_link_source;
        unsigned int _link_source_output;
        float _linking_x;
        float _linking_y;

        //  socket highlight
        NVGcolor _socket_highlight_color;
        bool _socket_highlighting{false};
        float _socket_highlight_x;
        float _socket_highlight_y;

        // link highlight
        const node_widget *_last_focused{nullptr};

        //  colors theme
        NVGcolor _link_color;
        NVGcolor _linking_color;
        NVGcolor _output_highlight_color;
        NVGcolor _input_highlight_color;
    };

} // namespace Gammou

#endif