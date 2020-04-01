
#include <stdio.h>

#include "circuit_editor.h"

namespace Gammou {

    /**
     *
     *      node widget implementation
     *
     **/
    static auto widget_node_height(DSPJIT::compile_node_class& node)
    {
        const auto io_count = std::max(node.get_input_count(), node.get_output_count());
        return static_cast<float>(io_count) * node_widget::socket_size +
            node_widget::node_header_size + node_widget::node_corner_radius;
    }

    //  Draw helper
    node_widget::node_widget(const std::string& name, DSPJIT::compile_node_class& node)
    : View::panel_implementation<>{node_width, widget_node_height(node)},
        _name{name},
        _node{node},
        _input_names{node.get_input_count()},
        _output_names{node.get_output_count()}
    {
        const auto ic = _input_names.size();
        const auto oc = _output_names.size();

        for (auto i = 0u; i < ic; ++i)
            _input_names[i] = _default_input_name(i);

        for (auto i = 0u; i < oc; ++i)
            _output_names[i] = _default_output_name(i);

        apply_color_theme(View::default_color_theme);
    }

    void node_widget::apply_color_theme(const View::color_theme& theme)
    {
        _text_color = theme.on_surface;
        _border_color = theme.secondary;
        _background_color = theme.surface;
        _socket_color = theme.primary;
    }

    void node_widget::set_input_name(unsigned int input_id, const std::string& name)
    {
        if (input_id < _input_names.size())
            _input_names[input_id] = name;
    }

    void node_widget::set_output_name(unsigned int output_id, const std::string& name)
    {
        if (output_id < _output_names.size())
            _output_names[output_id] = name;
    }

    void node_widget::draw(cairo_t *cr)
    {
        View::rounded_rectangle(cr, 0.f, 0.f, width(), height(), node_corner_radius);
        cairo_set_line_width(cr, 0.1f);

        //  Draw background
        View::set_source(cr, _background_color);
        cairo_fill_preserve(cr);

        //  Draw border
        View::set_source(cr, _border_color);
        cairo_stroke(cr);

        //  Draw name
        View::set_source(cr, _text_color);
        View::draw_text(cr, 0, 0, width(), node_header_size, 1.f, _name.c_str(), true);

        //  Draw inputs/outpus sockets and names
        const auto ic = _node.get_input_count();
        const auto oc = _node.get_output_count();

        for (auto i = 0u; i < ic; i++) {
            // draw socket
            float xc, yc;
            _input_pos(i, xc, yc);
            View::set_source(cr, _socket_color);
            View::circle(cr, xc, yc, socket_radius);

            if (nullptr != _node.get_input(i))
                cairo_fill(cr);
            else
                cairo_stroke(cr);

            //  draw input name
            const auto y_offset = node_header_size + socket_size * i;
            View::set_source(cr, _text_color);
            View::draw_text(
                cr, socket_size, y_offset, width(), socket_size,
                1.f, _input_names[i].c_str(), false,
                View::horizontal_alignment::left,
                View::vertical_alignment::top);
        }

        for (auto i = 0u; i < oc; i++) {
            // draw socket
            float xc, yc;
            _output_pos(i, xc, yc);
            View::set_source(cr, _socket_color);
            View::circle(cr, xc, yc, socket_radius);
            cairo_fill(cr);

            //  draw input name
            const auto y_offset = node_header_size + socket_size * i;
            View::set_source(cr, _text_color);
            View::draw_text(
                cr, 0, y_offset, width() - socket_size, socket_size,
                1.f, _output_names[i].c_str(), false,
                View::horizontal_alignment::right,
                View::vertical_alignment::top);
        }

        //  Draw childrens
        panel_implementation<>::draw(cr);
    }

    bool node_widget::_input_id_at(unsigned int& input_id, float x, float y)
    {
        const auto ic = _node.get_input_count();

        if (ic != 0u && x <= socket_hitbox_size) {
            input_id = std::clamp(static_cast<unsigned int>((y - node_header_size) / socket_size), 0u, ic - 1u);
            return true;
        }
        else {
            return false;
        }
    }

    bool node_widget::_output_id_at(unsigned int& output_id, float x, float y)
    {
        const auto oc = _node.get_output_count();

        if (oc != 0u && x >= (width() - socket_hitbox_size)) {
            output_id = std::clamp(static_cast<unsigned int>((y - node_header_size) / socket_size), 0u, oc - 1u);
            return true;
        }
        else {
            return false;
        }
    }

    void node_widget::_input_pos(unsigned int input_id, float& x, float& y)
    {
        x = socket_size / 2.f;
        y = node_header_size + input_id * socket_size + (socket_size / 2.f);
    }

    void node_widget::_output_pos(unsigned int output_id, float& x, float& y)
    {
        x = width() - socket_size / 2.f;
        y = node_header_size + output_id * socket_size + (socket_size / 2.f);
    }

    /**
     *
     *      circuit editor implementation
     *
     **/

    circuit_editor::circuit_editor(float width, float height)
    :   panel_implementation{width, height}
    {
        apply_color_theme(View::default_color_theme);
    }

    circuit_editor::circuit_editor(float width, float height, View::size_constraint width_constraint, View::size_constraint height_constraint)
    :   panel_implementation{width, height, width_constraint, height_constraint}
    {
        apply_color_theme(View::default_color_theme);
    }

    void circuit_editor::set_create_node_callback(create_node_callback cb)
    {
        _create_node_callback = cb;
    }

    void circuit_editor::set_circuit_changed_callback(circuit_changed_callback cb)
    {
        _circuit_changed_callback = cb;
    }

    void circuit_editor::insert_node_widget(float x, float y, std::unique_ptr<node_widget>&& w)
    {
        _node_widgets.emplace(&(w->node()), w.get());
        View::panel_implementation<node_widget>::insert_widget(x, y, std::move(w));
    }

    void circuit_editor::remove_node_widget(node_widget *children)
    {
        _socket_highlighting = false;
        _node_widgets.erase(&(children->node()));
        View::panel_implementation<node_widget>::remove_widget(children);
    }

    bool circuit_editor::on_mouse_dbl_click(float x, float y)
    {
        if (!panel_implementation<node_widget>::on_mouse_dbl_click(x, y)) {
            auto w = panel_implementation<node_widget>::widget_at(x, y);

            if (w != nullptr) {
                remove_node_widget(w->get());
                _notify_circuit_change();
                return true;
            }
            else if (_create_node_callback) {
                auto node = _create_node_callback();
                const auto insert_x = x - node->width() / 2.f;
                const auto insert_y = y - node->height() / 2.f;
                insert_node_widget(insert_x, insert_y, std::move(node));
                return true;
            }
            else {
                return false;
            }
        }
        else {
            return true;
        }
    }

    bool circuit_editor::on_mouse_move(float x, float y)
    {
        auto w = focused_widget();
        View::panel_implementation<node_widget>::on_mouse_move(x, y);

        if (w != nullptr) {
            auto node = w->get();
            unsigned int output_id;
            if (node->_output_id_at(output_id, x - node->pos_x(), y - node->pos_y())) {
                float cx, cy;
                node->_output_pos(output_id, cx, cy);
                _socket_highlighting = true;
                _socket_highlight_x = cx + node->pos_x();
                _socket_highlight_y = cy + node->pos_y();
                invalidate();
                return true;
            }
        }

        if (_socket_highlighting == true) {
            _socket_highlighting = false;
            invalidate();
        }

        return true;
    }

    bool circuit_editor::on_mouse_drag(const View::mouse_button button, float x, float y, float dx, float dy)
    {
        if (_is_linking) {
            _linking_x = x;
            _linking_y = y;
            _socket_highlighting = false;

            auto under_cursor = widget_at(x, y);
            if (under_cursor != nullptr) {
                auto node = under_cursor->get();
                unsigned int input_id;

                if (node->_input_id_at(input_id, x - node->pos_x(), y - node->pos_y())) {
                    float cx, cy;
                    node->_input_pos(input_id, cx, cy);
                    _socket_highlighting = true;
                    _socket_highlight_x = cx + node->pos_x();
                    _socket_highlight_y = cy + node->pos_y();
                }
            }

            invalidate();
            return true;
        }
        else {
            //  if a widget is focused : translate it
            if (auto w = focused_widget()) {
                w->set_pos(w->pos_x() + dx, w->pos_y() + dy);
                invalidate();
                return true;
            }
            else {
                return false;
            }
        }
    }

    bool circuit_editor::on_mouse_drag_start(const View::mouse_button button, float x, float y)
    {
        _socket_highlighting = false;

        if (button == View::mouse_button::left) {
            auto holder_ptr = widget_at(x, y);

            if (holder_ptr != nullptr) {
                auto& holder = *holder_ptr;
                unsigned int output_id;

                //  start a link if one output is under the cursor
                if (holder->_output_id_at(output_id, x - holder.pos_x(), y - holder.pos_y())) {
                    _link_source = holder.get();
                    _link_source_output = output_id;
                    _linking_x = x;
                    _linking_y = y;
                    _is_linking = true;
                }
            }
            else {
                /** @todo move the map ?? Non ailleur, pas le role de l'editor
                  **/
            }

            return true;
        }
        return false;
    }

    bool circuit_editor::on_mouse_drag_end(const View::mouse_button button, float x, float y)
    {
        //  Since drag event are not transferred to the panel implementation,
        //  we must inform the panel that cursor moved
        panel_implementation<node_widget>::on_mouse_move(x, y);

        if (button == View::mouse_button::left && _is_linking) {
            auto holder_ptr = widget_at(x, y);

            if (holder_ptr != nullptr) {
                auto& holder = *holder_ptr;
                unsigned int input_id;

                //  link if one of the node's input is under the cursor
                if (holder->_input_id_at(input_id, x - holder.pos_x(), y - holder.pos_y())) {
                    _link_source->node().connect(_link_source_output, holder->node(), input_id);
                    _notify_circuit_change();
                }
            }

            invalidate();
            _is_linking = false;
            _socket_highlighting = false;
            return true;
        }
        else {
            return false;
        }
    }

    bool circuit_editor::on_mouse_drag_cancel()
    {
        if (_is_linking) {
            _is_linking = false;
            _socket_highlighting = false;
            invalidate();
        }
        return true;
    }

    bool circuit_editor::on_mouse_button_up(const View::mouse_button button, float x, float y)
    {
        if (!View::panel_implementation<node_widget>::on_mouse_button_up(button, x, y)
                && button == View::mouse_button::right) {

            if (auto w = focused_widget()) {
                unsigned int input_id;
                auto node = w->get();
                if (node->_input_id_at(input_id, x - node->pos_x(), y - node->pos_y())) {
                    node->node().disconnect(input_id);
                    invalidate();
                    _notify_circuit_change();
                    return true;
                }
            }

            return false;
        }
        else {
            return true;
        }
    }

    void circuit_editor::apply_color_theme(const View::color_theme& theme)
    {
        _link_color = theme.primary_light;
        _linking_color = theme.secondary_light;
        _socket_highlight_color = theme.secondary_light;
    }

    void circuit_editor::draw(cairo_t *cr)
    {
        //  Draw nodes
        panel_implementation<node_widget>::draw(cr);

        //  Draw node links
        for (const auto& holder : _childrens) {
            const auto& node = holder->node();
            const auto input_count = node.get_input_count();

            //  Draw links to each linled input
            for (auto i = 0u; i < input_count; ++i) {
                unsigned int output_id;
                const auto input_node = node.get_input(i, output_id);

                if (input_node != nullptr) {
                    auto it = _node_widgets.find(input_node);

                    if (it != _node_widgets.end()) {
                        //  get coresponding node_widget
                        const auto input_node_widget = it->second;
                        float output_x, output_y, input_x, input_y;

                        input_node_widget->_output_pos(output_id, output_x, output_y);
                        holder->_input_pos(i, input_x, input_y);

                        //  draw link
                        _draw_link(cr, _link_color,
                            output_x + input_node_widget->pos_x(), output_y + input_node_widget->pos_y(),
                            input_x + holder.pos_x(), input_y + holder.pos_y());
                    }
                }
            }
        }

        //  If linking, draw new link
        if (_is_linking) {
            float output_x, output_y;
            _link_source->_output_pos(_link_source_output, output_x, output_y);
            _draw_link(cr, _linking_color,
                output_x + _link_source->pos_x(), output_y + _link_source->pos_y(),
                _linking_x, _linking_y);
        }

        // If a socket is highlighted
        if (_socket_highlighting) {
            View::circle(cr, _socket_highlight_x, _socket_highlight_y, node_widget::socket_radius * 1.3f);
            View::set_source(cr, _socket_highlight_color);
            cairo_fill(cr);
        }
    }

    void circuit_editor::_draw_link(
        cairo_t *cr, View::color color,
        float x_output, float y_output,
        float x_input, float y_input) const noexcept
    {
        float D = std::min<float>(std::fabs(x_input - x_output), x_input >= x_output ? 5.f : 10.f);

		cairo_move_to(cr, x_output, y_output);
		cairo_curve_to(cr, x_output + D, y_output, x_input - D, y_input, x_input, y_input);

		cairo_set_line_width(cr, 0.2f);
        cairo_set_line_cap(cr, CAIRO_LINE_CAP_ROUND);

		View::set_source(cr, color);
		cairo_stroke(cr);
    }

    void circuit_editor::_notify_circuit_change()
    {
        if (_circuit_changed_callback)
            _circuit_changed_callback();
    }


}