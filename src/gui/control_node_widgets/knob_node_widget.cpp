
#include "knob_node_widget.h"
#include "test_implentations.h"

namespace Gammou {

    class knob_node_widget : public owning_node_widget {
        using parameter = synthesizer::parameter;
    public:
        knob_node_widget(parameter && param)
        :   owning_node_widget{
                "Knob",
                std::make_unique<DSPJIT::reference_compile_node>(param.get_value_ref())
            },
            _param{std::move(param)}
        {
            //  To make place for the knob widget
            set_output_name(0u, "");

            //  Create the knob and link it to the parameter
            auto knob = std::make_unique<View::knob>();
            knob->set_callback([this](float val) { _param.set(val);});
            knob->set_value(*_param.get_value_ref());

            //  Insert the knob widget
            resize_height(node_widget::node_header_size * 2.f + knob->height());
            insert_widget(
                (width() - knob->width()) / 2,
                node_widget::node_header_size * 1.5f,
                std::move(knob));
        }

    private:
        parameter _param;
    };

    knob_node_widget_plugin::knob_node_widget_plugin(synthesizer& synth)
    :   _synth{synth}
    {
        _name = "Knob";
        _category = "Control";
    }

    std::unique_ptr<node_widget> knob_node_widget_plugin::create_node()
    {
        return std::make_unique<knob_node_widget>(_synth.allocate_parameter());
    }
}