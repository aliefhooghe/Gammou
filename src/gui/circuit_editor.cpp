#include <DSPJIT/log.h>

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

    void node_widget::set_name(const std::string& name)
    {
        _name = name;
        invalidate();
    }

    void node_widget::set_input_name(unsigned int input_id, const std::string& name)
    {
        _check_io_counts();
        if (input_id < _input_names.size()) {
            _input_names[input_id] = name;
            invalidate();
        }
    }

    void node_widget::set_output_name(unsigned int output_id, const std::string& name)
    {
        _check_io_counts();
        if (output_id < _output_names.size()) {
            _output_names[output_id] = name;
            invalidate();
        }
    }

    const std::string& node_widget::get_input_name(const unsigned int input_id)
    {
        _check_io_counts();
        if (input_id < _input_names.size())
            return _input_names[input_id];
        else
            throw std::out_of_range("get_input_name: bad input id");
    }

    const std::string& node_widget::get_output_name(const unsigned int output_id)
    {
        _check_io_counts();
        if (output_id < _output_names.size())
            return _output_names[output_id];
        else
            throw std::out_of_range("get_output_name: bad output_id");
    }

     unsigned int node_widget::get_input_count()
    {
        _check_io_counts();
        return _input_names.size();
    }

     unsigned int node_widget::get_output_count()
    {
        _check_io_counts();
        return _output_names.size();
    }

    void node_widget::draw(NVGcontext *vg)
    {
        // Check if IO have been added/removed
        _check_io_counts();

        nvgBeginPath(vg);
        nvgRoundedRect(vg, 0.f, 0.f, width(), height(), node_corner_radius);

        //  Draw background
        nvgFillColor(vg, _background_color);
        nvgFill(vg);

        //  Draw border
        nvgStrokeColor(vg, _border_color);
        nvgStrokeWidth(vg, 1.5f);
        nvgStroke(vg);

        //  Draw name
        nvgFillColor(vg, _text_color);
        View::draw_text(vg, 0, 0, width(), node_header_size, 14, _name.c_str(), true,
            View::horizontal_alignment::center, View::vertical_alignment::bottom);

        //  Draw inputs/outpus sockets and names
        const auto ic = _node.get_input_count();
        const auto oc = _node.get_output_count();

        for (auto i = 0u; i < ic; i++) {
            // draw socket
            float xc, yc;
            _input_pos(i, xc, yc);

            nvgBeginPath(vg);
            nvgCircle(vg,  xc, yc, socket_radius);

            if (nullptr != _node.get_input(i)) {
                nvgFillColor(vg, _socket_color);
                nvgFill(vg);
            }
            else {
                nvgStrokeColor(vg, _socket_color);
                nvgStroke(vg);
            }

            //  draw input name
            const auto y_offset = node_header_size + socket_size * i;
            nvgFillColor(vg, _text_color);
            View::draw_text(
                vg, socket_size, y_offset, width(), socket_size,
                13, _input_names[i].c_str(), false,
                View::horizontal_alignment::left,
                View::vertical_alignment::top);
        }

        for (auto i = 0u; i < oc; i++) {
            // draw socket
            float xc, yc;
            _output_pos(i, xc, yc);

            nvgBeginPath(vg);
            nvgCircle(vg,  xc, yc, socket_radius);
            nvgFillColor(vg, _socket_color);
            nvgFill(vg);

            //  draw input name
            const auto y_offset = node_header_size + socket_size * i;
            nvgFillColor(vg, _text_color);
            View::draw_text(
                vg, 0, y_offset, width() - socket_size, socket_size,
                13, _output_names[i].c_str(), false,
                View::horizontal_alignment::right,
                View::vertical_alignment::top);
        }

        //  Draw childrens
        panel_implementation<>::draw(vg);
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

    template <typename TNameGen>
    static bool _check_io_name_vector(std::vector<std::string>& names, unsigned int node_io_count, TNameGen gen)
    {
        const auto gui_io_count = names.size();
        if (gui_io_count != node_io_count) {
            names.resize(node_io_count);
            for (auto i = gui_io_count; i < node_io_count; i++)
                names[i] = gen(i);
            return true;
        }
        else {
            return false;
        }
    }

    void node_widget::_check_io_counts()
    {
        const auto input_count_changed =
            _check_io_name_vector(_input_names, _node.get_input_count(), _default_input_name);
        const auto output_count_changed =
            _check_io_name_vector(_output_names, _node.get_output_count(), _default_output_name);

        if (input_count_changed || output_count_changed)
            resize_height(widget_node_height(_node));
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

    void circuit_editor::clear()
    {
        std::vector<node_widget*> to_delete;
        to_delete.reserve(_childrens.size());

        //  list the node to be removed and unlink every links
        for (auto& child : _childrens) {
            to_delete.push_back(child.get());

            const auto ic = child->node().get_input_count();
            for (auto i = 0u; i < ic; ++i)
                child->node().disconnect(i);
        }

        for (auto node : to_delete)
            remove_node_widget(node);

        invalidate();
    }

    bool circuit_editor::on_mouse_move(float x, float y)
    {
        View::panel_implementation<node_widget>::on_mouse_move(x, y);
        auto holder = focused_widget();

        if (holder != nullptr) {
            auto focused_node = holder->get();

            if (focused_node != _last_focused) {
                _last_focused = focused_node;
                invalidate();
            }

            unsigned int io_id;
            float cx, cy;

            if (focused_node->_output_id_at(io_id, x - focused_node->pos_x(), y - focused_node->pos_y()))
            {
                focused_node->_output_pos(io_id, cx, cy);
                _socket_highlight_color = _output_highlight_color;
                _socket_highlighting = true;
            }
            else if (focused_node->_input_id_at(io_id, x - focused_node->pos_x(), y - focused_node->pos_y()) &&
                    focused_node->node().get_input(io_id) != nullptr)
            {
                focused_node->_input_pos(io_id, cx, cy);
                _socket_highlight_color = _input_highlight_color;
                _socket_highlighting = true;
            }
            else if (_socket_highlighting) {
                _socket_highlighting = false;
                invalidate();
            }

            if (_socket_highlighting) {
                _socket_highlight_x = cx + focused_node->pos_x();
                _socket_highlight_y = cy + focused_node->pos_y();
                invalidate();
            }

            return true;
        }
        else {
            if (_last_focused != nullptr || _socket_highlighting == true) {
                _last_focused = nullptr;
                _socket_highlighting = false;
                invalidate();
            }

            return false;
        }
    }

    bool circuit_editor::on_mouse_drag(const View::mouse_button button, float x, float y, float dx, float dy)
    {
        if (_drag_state == drag_state::DRAG_NODE)
        {
            return View::panel_implementation<node_widget>::on_mouse_drag(button, x, y, dx, dy);
        }
        else if (_drag_state == drag_state::LINK_NODE)
        {
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
        else if (_drag_state == drag_state::MOVE_NODE)
        {
            if (auto w = focused_widget())
            {
                w->set_pos(w->pos_x() + dx, w->pos_y() + dy);
                invalidate();
                return true;
            }
            else
            {
                LOG_WARNING("[circuit editor][drag] LOST MOVING WIDGET\n");
                _drag_state = drag_state::OFF;
                return false;
            }
        }
        else {  // OFF
            return false;

        }
    }

    bool circuit_editor::on_mouse_drag_start(const View::mouse_button button, float x, float y)
    {
        _socket_highlighting = false;

        if (!View::panel_implementation<node_widget>::on_mouse_drag_start(button, x, y)) {

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
                        _drag_state = drag_state::LINK_NODE;
                        return true;
                    }
                    else {
                        _drag_state = drag_state::MOVE_NODE;
                        return true;
                    }
                }
                else {
                    /** @todo move the map ?? Non ailleur, pas le role de l'editor
                     **/
                    _drag_state = drag_state::OFF;
                    return false;
                }

            }
            else {
                _drag_state = drag_state::OFF;  // ignore other buttons
                return false;
            }
        }
        else {
            _drag_state = drag_state::DRAG_NODE;
            return true;
        }
    }

    bool circuit_editor::on_mouse_drag_end(const View::mouse_button button, float x, float y)
    {
        const auto previous_state = _drag_state;
        _drag_state = drag_state::OFF;

        if (previous_state == drag_state::DRAG_NODE)
        {
            return View::panel_implementation<node_widget>::on_mouse_drag_end(button, x, y);
        }
        else if (previous_state == drag_state::LINK_NODE)
        {
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
            _socket_highlighting = false;
            return true;
        }
        else {
            return false;
        }
    }

    bool circuit_editor::on_mouse_drag_cancel()
    {
        View::panel_implementation<node_widget>::on_mouse_drag_cancel();

        if (_drag_state == drag_state::LINK_NODE) {
            _socket_highlighting = false;
            invalidate();
        }

        _drag_state = drag_state::OFF;
        return true;
    }

    bool circuit_editor::on_mouse_button_up(const View::mouse_button button, float x, float y)
    {
        if (!View::panel_implementation<node_widget>::on_mouse_button_up(button, x, y))
        {
            if (auto w = focused_widget()) {
                auto widget = w->get();
                // delete focused node with right click
                if (button == View::mouse_button::right) {
                    unsigned int input_id;

                    if (widget->_input_id_at(input_id, x - widget->pos_x(), y - widget->pos_y()) &&
                        widget->node().get_input(input_id) != nullptr) {
                        widget->node().disconnect(input_id);
                    }
                    else {
                        if (!w->get()->is_internal())
                            remove_node_widget(w->get());
                    }

                    _notify_circuit_change();
                    invalidate();
                    return true;
                }
            }
            // nothing focused : create node with left click
            else if (button == View::mouse_button::left && _create_node_callback) {

                try
                {
                    auto node = _create_node_callback();

                    if (node) {
                        const auto insert_x = x - node->width() / 2.f;
                        const auto insert_y = y - node->height() / 2.f;
                        insert_node_widget(insert_x, insert_y, std::move(node));
                    }
                }
                catch (const std::exception& e)
                {
                    LOG_ERROR("[circuit_editor] Failed to create a node: %s\n", e.what());
                }

                return true;
            }
        }

        return true;
    }

    void circuit_editor::apply_color_theme(const View::color_theme& theme)
    {
        _link_color = theme.primary_light;
        _linking_color = theme.secondary_light;
        _output_highlight_color = theme.secondary_light;
        _input_highlight_color = theme.error;
    }

    void circuit_editor::draw(NVGcontext *vg)
    {
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

                        const bool highlight_link =
                            (_drag_state != drag_state::LINK_NODE) &&
                            (_last_focused == input_node_widget || _last_focused == holder.get());

                        const auto link_width = 3.f;
                        const auto link_color = highlight_link ? _linking_color : _link_color;

                        //  draw link
                        _draw_link(vg, link_color,
                            output_x + input_node_widget->pos_x(), output_y + input_node_widget->pos_y(),
                            input_x + holder.pos_x(), input_y + holder.pos_y(), link_width);
                    }
                }
            }
        }

        //  Draw nodes
        panel_implementation<node_widget>::draw(vg);

        //  If linking, draw new link
        if (_drag_state == drag_state::LINK_NODE) {
            float output_x, output_y;
            _link_source->_output_pos(_link_source_output, output_x, output_y);
            _draw_link(vg, _linking_color,
                output_x + _link_source->pos_x(), output_y + _link_source->pos_y(),
                _linking_x, _linking_y, 3.f);
        }

        // If a socket is highlighted
        if (_socket_highlighting) {
            nvgBeginPath(vg);
            nvgCircle(vg, _socket_highlight_x, _socket_highlight_y, node_widget::socket_radius * 1.3f);
            nvgFillColor(vg, _socket_highlight_color);
            nvgFill(vg);
        }
    }

    void circuit_editor::_draw_link(
        NVGcontext *vg, NVGcolor color,
        float x_output, float y_output,
        float x_input, float y_input, float link_width) const noexcept
    {
        float D = std::min<float>(std::fabs(x_input - x_output), x_input >= x_output ? 50.f : 100.f);

        nvgBeginPath(vg);
        nvgMoveTo(vg, x_output, y_output);
        nvgBezierTo(vg, x_output + D, y_output, x_input - D, y_input, x_input, y_input);
        nvgStrokeWidth(vg, link_width);
        nvgStrokeColor(vg, color);
        nvgStroke(vg);
    }

    void circuit_editor::_notify_circuit_change()
    {
        if (_circuit_changed_callback)
            _circuit_changed_callback();
    }

    /*
     *
     *      Serialization / Deserialization
     *
     */

    struct node_desc {
        float x;
        float y;
        nlohmann::json node;
    };

    struct link_source_desc {
        unsigned int node;
        unsigned int out;
    };

    struct link_dest_desc {
        unsigned int node;
        unsigned int in;
    };
    struct link_desc {
        link_source_desc from;
        link_dest_desc to;
    };

    struct circuit_desc {
        std::vector<node_desc> nodes;
        std::vector<link_desc> links;
    };

    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(node_desc, x, y, node)
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(link_source_desc, node, out)
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(link_dest_desc, node, in)
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(link_desc, from, to)
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(circuit_desc, nodes, links)

    nlohmann::json circuit_editor::serialize()
    {
        std::unordered_map<const node_widget*, unsigned int> node_index{};
        circuit_desc desc;

        //  Store nodes
        desc.nodes.reserve(_childrens.size());

        for (auto i = 0u; i < _childrens.size(); ++i) {
            const auto& child = _childrens[i];
            desc.nodes.emplace_back(
                node_desc{
                    child.pos_x(),
                    child.pos_y(),
                    std::move(child->serialize())});
            node_index.emplace(child.get(), i);
        }

        //  Store links
        for (const auto& child : _childrens) {
            const auto child_index = node_index[child.get()];
            const auto& node = child->node();
            const auto ic = node.get_input_count();

            for (auto i = 0u; i < ic; ++i) {
                unsigned int output_id;
                const auto input_node = node.get_input(i, output_id);

                if (input_node != nullptr) {
                    auto it = _node_widgets.find(input_node);

                    if (it != _node_widgets.end()) {
                        desc.links.emplace_back(link_desc{
                            link_source_desc{node_index[it->second], output_id},
                            link_dest_desc{child_index, i}}
                        );
                    }
                }
            }
        }

        return desc;
    }

    void circuit_editor::deserialize(const nlohmann::json& json, node_deserializer deserializer)
    {
        std::unordered_map<unsigned int, node_widget*> node_index{};
        circuit_desc desc;
        from_json(json, desc);

        //  clear current content
        clear();

        //  Create nodes
        for (auto i = 0u; i < desc.nodes.size(); ++i) {
            const auto& node_desc = desc.nodes[i];
            auto node = deserializer(node_desc.node);
            node_index.emplace(i, node.get());
            insert_node_widget(
                node_desc.x,
                node_desc.y,
                std::move(node));
        }

        //  create links
        for (const auto& link_desc : desc.links) {
            node_index[link_desc.from.node]->node().connect(
                link_desc.from.out,
                node_index[link_desc.to.node]->node(),
                link_desc.to.in);
        }

        // caller is responsible of recompilation
    }
}