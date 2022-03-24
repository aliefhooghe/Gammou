
#ifndef GAMMOU_IO_NAMING_TOOLBOX_H_
#define GAMMOU_IO_NAMING_TOOLBOX_H_

#include "gui/circuit_editor.h"

namespace Gammou
{
    class io_naming_toolbox : public View::panel_implementation<>
    {
        static constexpr auto _internal_border_size = 5.f;
        static constexpr auto _text_input_width =
            View::text_input::default_width / 2.f;

    public:
        io_naming_toolbox(
            node_widget& node,
            float height);

        void update();

    private:
        void _update_text_input_vector(
            std::vector<View::text_input*>& vector,
            unsigned int current_count, float x_offset);

        node_widget& _node;
        std::vector<View::text_input*> _input_names{};
        std::vector<View::text_input*> _output_names{};
    };
}

#endif