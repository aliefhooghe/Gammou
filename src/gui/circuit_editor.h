#ifndef GAMMOU_CIRCUIT_EDITOR_H_
#define GAMMOU_CIRCUIT_EDITOR_H_

#include <functional>
#include <memory>
#include <unordered_map>
#include <vector>
#include <string>

#include <compile_node_class.h>   //  DSPJIT nodes
#include <view.h>


namespace Gammou
{

    class circuit_editor;

    class node_widget : public View::panel<View::widget> {
        friend class circuit_editor;
    public:
        node_widget(const std::string& name, DSPJIT::compile_node_class&);
        ~node_widget() override = default;

        void apply_color_theme(const View::color_theme& theme) override;

        void set_input_name(unsigned int input_id, const std::string& name);
        void set_output_name(unsigned int output_id, const std::string& name);

        const auto& get_input_name(const unsigned int input_id) const noexcept { return _input_names[input_id]; }
        const auto& get_output_name(const unsigned int output_id) const noexcept { return _output_names[output_id]; }

        auto& node() const noexcept { return _node; }
        auto& node() noexcept { return _node; }

    protected:
        void draw_background(cairo_t *cr) override;

    private:
        auto _default_input_name(unsigned int idx) { return "Output-" + std::to_string(idx); }
        auto _default_output_name(unsigned int idx) { return "Input-" + std::to_string(idx); }

        bool _input_id_at(unsigned int& input_id, float x, float y);
        bool _output_id_at(unsigned int& output_id, float x, float y);

        void _input_pos(unsigned int input_id, float& x, float& y);
        void _output_pos(unsigned int output_id, float& x, float& y);

        DSPJIT::compile_node_class& _node;
        std::string _name;
        std::vector<std::string> _input_names;
        std::vector<std::string> _output_names;

        //  colors
        View::color _text_color;
        View::color _border_color;
        View::color _background_color;
        View::color _socket_color;
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

        /**
         *  Vraiment ? pas connecté avec une factory ?
         *  Comment gerrer les configs ? (ouverture de renetre modale)
         **/
        using create_callback =
            std::function<std::unique_ptr<node_widget>(void)>;

        public:
            circuit_editor(float width, float height);
            circuit_editor(float width, float height, View::size_constraint width_constraint, View::size_constraint height_constraint);
            void set_create_callback(create_callback);

            void insert_node_widget(float x, float y, std::unique_ptr<node_widget>&&);
            void remove_node_widget(node_widget*);

            bool on_mouse_dbl_click(float x, float y) override;
            bool on_mouse_move(float x, float y) override;
            bool on_mouse_drag(const View::mouse_button button, float x, float y, float dx, float dy) override;
            bool on_mouse_drag_start(const View::mouse_button button, float x, float y) override;
            bool on_mouse_drag_end(const View::mouse_button button, float x, float y) override;
            bool on_mouse_drag_cancel() override;

            void apply_color_theme(const View::color_theme& theme) override;

        protected:
            // void draw_background(cairo_t *cr) override;
            void draw_foreground(cairo_t *cr) override;
            void draw_link(
                cairo_t *cr, View::color color,
                float x_input, float y_input,
                float x_output, float y_output) const noexcept;

        private:
            create_callback _create_callback{};
            std::unordered_map<const DSPJIT::compile_node_class*, node_widget*> _node_widgets{};

            //  link state machine
            bool _is_linking{false};
            node_widget *_link_source;
			unsigned int _link_source_output;
			float _linking_x;
			float _linking_y;

            //  socket highlight
            bool _socket_highlighting{false};
            float _socket_highlight_x;
            float _socket_highlight_y;

            //  colors
            View::color _link_color;
            View::color _linking_color;
            View::color _socket_highlight_color;
    };


} // namespace Gammou


#endif