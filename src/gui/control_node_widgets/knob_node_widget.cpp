
#include "knob_node_widget.h"
#include "common_nodes.h"
#include "parameter_serialization.h"

namespace Gammou {

    static constexpr auto value_knob_widget_uid = 0x384d61a1be4de6cdu;
    static constexpr auto gain_knob_widget_uid = 0xdde47c1126a20041u;

    class knob_node_widget : public plugin_node_widget {
        using parameter = synthesizer::parameter;
    public:

        enum class mode {
            VALUE, GAIN
        };

        knob_node_widget(synthesizer& synth, mode m, parameter && param)
        :   plugin_node_widget{
                "Knob", m == mode::VALUE ? value_knob_widget_uid : gain_knob_widget_uid,
                make_compile_node(m, param)
            },
            _param{std::move(param)},
            _synthesizer{synth}
        {
            //  To make place for the knob widget
            set_output_name(0u, "");
            if (m == mode::GAIN)
                set_input_name(0u, "");

            //  Create the knob and link it to the parameter
            auto knob = std::make_unique<View::knob>();
            knob->set_callback([this](float val) { _param.set_normalized(val);});
            knob->set_value(_param.get_normalized());

            //  Create the midi learn button
            auto learn_button = std::make_unique<View::text_push_button>("L", node_widget::node_header_size, node_widget::node_header_size);
            learn_button->set_callback([this, &synth](){ synth.midi_learn(_param); });

            // Create the scale buttons
            auto button_scale_up = std::make_unique<View::text_push_button>("+", node_widget::node_header_size, node_widget::node_header_size);
            auto button_scale_down = std::make_unique<View::text_push_button>("-", node_widget::node_header_size, node_widget::node_header_size);

            button_scale_up->set_callback([this](){ _param.set_shape_scale(_param.get_shape_scale() * 4.f); });
            button_scale_down->set_callback([this](){ _param.set_shape_scale(_param.get_shape_scale() / 4.f); });

            //  Insert the widgets
            resize_height(node_widget::node_header_size * 2.f + knob->height());

            const auto knob_x_pos = width() - node_widget::node_header_size - knob->width();
            insert_widget(
                knob_x_pos,
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
            return parameter_to_json(_synthesizer, _param);
        }

    private:
        static std::unique_ptr<DSPJIT::compile_node_class> make_compile_node(mode m, parameter& param)
        {
            if (m == mode::VALUE)
                return std::make_unique<DSPJIT::reference_node>(param.get_value_ptr());
            else // GAIN
                return std::make_unique<DSPJIT::reference_multiply_node>(param.get_value_ptr());
        }

        parameter _param;
        synthesizer& _synthesizer;
    };

    /**
     * \brief value_knob_node_widget_plugin
     */
    value_knob_node_widget_plugin::value_knob_node_widget_plugin(synthesizer& synth)
    :   node_widget_factory::plugin{value_knob_widget_uid, "Knob" ,"Control"},
        _synth{synth}
    {
    }

    std::unique_ptr<plugin_node_widget> value_knob_node_widget_plugin::create_node(circuit_tree_model&)
    {
        return std::make_unique<knob_node_widget>(
            _synth,
            knob_node_widget::mode::VALUE,
            _synth.allocate_parameter(0.f));
    }

    std::unique_ptr<plugin_node_widget> value_knob_node_widget_plugin::create_node(circuit_tree_model&, const nlohmann::json& json)
    {
        auto param = parameter_from_json(json, _synth);
        return std::make_unique<knob_node_widget>(
            _synth,
            knob_node_widget::mode::VALUE,
            std::move(param));
    }

    /**
     * \brief gain_knob_node_widget_plugin
     */
    gain_knob_node_widget_plugin::gain_knob_node_widget_plugin(synthesizer& synth)
    :   node_widget_factory::plugin{gain_knob_widget_uid, "Gain Knob" ,"Control"},
        _synth{synth}
    {
    }

    std::unique_ptr<plugin_node_widget> gain_knob_node_widget_plugin::create_node(circuit_tree_model&)
    {
        return std::make_unique<knob_node_widget>(
            _synth,
            knob_node_widget::mode::GAIN,
            _synth.allocate_parameter(0.f));
    }

    std::unique_ptr<plugin_node_widget> gain_knob_node_widget_plugin::create_node(circuit_tree_model&, const nlohmann::json& json)
    {
        auto param = parameter_from_json(json, _synth);
        return std::make_unique<knob_node_widget>(
            _synth,
            knob_node_widget::mode::GAIN,
            std::move(param));
    }
}