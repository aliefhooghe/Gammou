
#include "knob_node_widget.h"
#include "test_implentations.h"

namespace Gammou {

    class knob_node_widget : public plugin_node_widget {
        using parameter = synthesizer::parameter;
    public:
        knob_node_widget(synthesizer& synth, parameter && param)
        :   plugin_node_widget{
                "Knob", 12,
                std::make_unique<DSPJIT::reference_compile_node>(param.get_value_ptr())
            },
            _param{std::move(param)}
        {
            //  To make place for the knob widget
            set_output_name(0u, "");

            //  Create the knob and link it to the parameter
            auto knob = std::make_unique<View::knob>();
            knob->set_callback([this](float val) { _param.set_normalized(val);});
            knob->set_value(*_param.get_value_ptr());

            //  Create the midi learn button
            auto learn_button = std::make_unique<View::text_push_button>("L", 1.5f, 1.5f);
            learn_button->set_callback([this, &synth](){ synth.midi_learn(_param); });

            // //  Create the scale buttons
            auto button_scale_up = std::make_unique<View::text_push_button>("+", 1.5f, 1.5f);
            auto button_scale_down = std::make_unique<View::text_push_button>("-", 1.5f, 1.5f);

            button_scale_up->set_callback([this](){ _param.set_shape_scale(_param.get_shape_scale() * 2.f); });
            button_scale_down->set_callback([this](){ _param.set_shape_scale(_param.get_shape_scale() / 2.f); });

            //  Insert the widgets
            resize_height(node_widget::node_header_size * 2.f + knob->height());
            insert_widget(
                width() - node_widget::node_header_size - knob->width(),
                node_widget::node_header_size * 1.5f,
                std::move(knob));

            insert_widget(
                node_widget::node_header_size, node_widget::node_header_size, std::move(learn_button));

            insert_widget(
                node_widget::node_header_size, node_widget::node_header_size * 2, std::move(button_scale_up));

            insert_widget(
                node_widget::node_header_size, node_widget::node_header_size * 3, std::move(button_scale_down));

        }

    private:
        parameter _param;
    };

    knob_node_widget_plugin::knob_node_widget_plugin(synthesizer& synth)
    :   node_widget_factory::plugin{12, "Knob" ,"Control"},
        _synth{synth}
    {
    }

    std::unique_ptr<plugin_node_widget> knob_node_widget_plugin::create_node()
    {
        return std::make_unique<knob_node_widget>(
            _synth, _synth.allocate_parameter());
    }
}