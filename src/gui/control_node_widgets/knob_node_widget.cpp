
#include "knob_node_widget.h"
#include "test_implentations.h"

namespace Gammou {

    struct parameter_descriptor
    {
        float normalized;
        float shape_base;
        float shape_scale;
        std::optional<uint8_t> control;
    };

    void from_json(const nlohmann::json& json, parameter_descriptor& desc)
    {
        json.at("normalized").get_to(desc.normalized);
        json.at("shape-base").get_to(desc.shape_base);
        json.at("shape-scale").get_to(desc.shape_scale);

        auto it = json.find("control");
        if (it != json.end())
            desc.control = it->get<uint8_t>();
    }

    void to_json(nlohmann::json& json, const parameter_descriptor& desc)
    {
        json = {
            {"normalized", desc.normalized},
            {"shape-base", desc.shape_base},
            {"shape-scale", desc.shape_scale}
        };

        if (desc.control)
            json["control"] = desc.control.value();
    }


    class knob_node_widget : public plugin_node_widget {
        using parameter = synthesizer::parameter;
    public:
        knob_node_widget(synthesizer& synth, parameter && param)
        :   plugin_node_widget{
                "Knob", 12,
                std::make_unique<DSPJIT::reference_compile_node>(param.get_value_ptr())
            },
            _param{std::move(param)},
            _synthesizer{synth}
        {
            //  To make place for the knob widget
            set_output_name(0u, "");

            //  Create the knob and link it to the parameter
            auto knob = std::make_unique<View::knob>();
            knob->set_callback([this](float val) { _param.set_normalized(val);});
            knob->set_value(_param.get_normalized());

            //  Create the midi learn button
            auto learn_button = std::make_unique<View::text_push_button>("L", 1.5f, 1.5f);
            learn_button->set_callback([this, &synth](){ synth.midi_learn(_param); });

            // Create the scale buttons
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

        nlohmann::json serialize_internal_state() override
        {
            parameter_descriptor desc;
            desc.normalized = _param.get_normalized();
            desc.shape_base = _param.get_shape_base();
            desc.shape_scale = _param.get_shape_scale();

            uint8_t control;
            if (_synthesizer.midi_assigned_to_control(control, _param))
                desc.control = control;

            nlohmann::json state;
            to_json(state, desc);
            return state;
        }

    private:
        parameter _param;
        synthesizer& _synthesizer;
    };

    knob_node_widget_plugin::knob_node_widget_plugin(synthesizer& synth)
    :   node_widget_factory::plugin{12, "Knob" ,"Control"},
        _synth{synth}
    {
    }

    std::unique_ptr<plugin_node_widget> knob_node_widget_plugin::create_node()
    {
        return std::make_unique<knob_node_widget>(
            _synth, _synth.allocate_parameter(0.f));
    }

    std::unique_ptr<plugin_node_widget> knob_node_widget_plugin::create_node(const nlohmann::json& json)
    {
        parameter_descriptor desc;
        from_json(json, desc);

        auto param = _synth.allocate_parameter(desc.normalized);
        param.set_shape_base(desc.shape_base);
        param.set_shape_scale(desc.shape_scale);

        if (desc.control)
            _synth.midi_assign_control(desc.control.value(), param);

        return std::make_unique<knob_node_widget>(_synth, std::move(param));
    }
}