
#include <algorithm>
#include "io_naming_toolbox.h"

namespace Gammou
{
    io_naming_toolbox::io_naming_toolbox(
        node_widget& node,
        float height)
    : panel_implementation<>{
        2.f * _text_input_width + _internal_border_size,
        height},
      _node{node}
    {
        auto apply_button =
            std::make_unique<View::text_push_button>(
                "rename I/O",
                width(),
                View::push_button::default_height);

        apply_button->set_callback(
            [this]()
            {
                // avoid out of bound renaming
                const auto input_count = std::min<std::size_t>(
                    _node.get_input_count(),
                    _input_names.size());
                const auto output_count = std::min<std::size_t>(
                    _node.get_output_count(),
                    _output_names.size());

                for (auto i = 0u; i < input_count; ++i)
                    _node.set_input_name(
                        i, _input_names[i]->get_text());
                for (auto i = 0u; i < output_count; ++i)
                    _node.set_output_name(
                        i, _output_names[i]->get_text());
            });

        insert_widget(
            0, 0,
            std::move(apply_button));

        update();
    }

    void io_naming_toolbox::update()
    {
        const auto input_count = _node.get_input_count();
        const auto output_count = _node.get_output_count();

        // Update text input vectors size
        _update_text_input_vector(
            _input_names, input_count, 0.f);

        _update_text_input_vector(
            _output_names, output_count,
            _text_input_width + _internal_border_size);

        // Update names
        for (auto i = 0u; i< input_count; ++i)
            _input_names[i]->set_text(
                _node.get_input_name(i));
        for (auto i = 0u; i< output_count; ++i)
            _output_names[i]->set_text(
                _node.get_output_name(i));
    }


    void io_naming_toolbox::_update_text_input_vector(
        std::vector<View::text_input*>& vector,
        unsigned int current_count,
        float x_offset)
    {
        const auto vector_size = vector.size();

        if (vector_size < current_count) {
            // Create missing text inputs
            for (auto i = vector_size; i < current_count; i++) {
                const float y_offset =
                    i * (View::text_input::default_height + _internal_border_size) +
                    View::push_button::default_height + _internal_border_size;
                auto text_input =
                    std::make_unique<View::text_input>(_text_input_width);

                // Add text input to the panel and keep a reference
                vector.push_back(text_input.get());
                insert_widget(
                    x_offset, y_offset,
                    std::move(text_input));
            }

        }
        else if (vector_size > current_count)
        {
            for (auto i = current_count; i < vector_size; i++)
                remove_widget(vector[i]);
            vector.resize(current_count);
        }
        // else: size is up to date
    }
}