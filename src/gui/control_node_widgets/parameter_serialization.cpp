
#include <optional>
#include <stdint.h>

#include "parameter_serialization.h"
#include "utils/serialization_helpers.h"

namespace Gammou {

    struct parameter_descriptor
    {
        float normalized{};
        float shape_base{};
        float shape_scale{};
        std::optional<uint8_t> midi_control{};
    };
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(parameter_descriptor, normalized, shape_base, shape_scale, midi_control)

    synthesizer::parameter parameter_from_json(const nlohmann::json& json, synthesizer& synth)
    {
        parameter_descriptor desc;
        from_json(json, desc);

        auto param = synth.allocate_parameter(desc.normalized);
        param.set_shape_base(desc.shape_base);
        param.set_shape_scale(desc.shape_scale);

        if (desc.midi_control.has_value())
            synth.midi_assign_control(desc.midi_control.value(), param);

        return param;
    }

    nlohmann::json parameter_to_json(const synthesizer& synth,const synthesizer::parameter& param)
    {
        parameter_descriptor desc{
            param.get_normalized(),
            param.get_shape_base(),
            param.get_shape_scale()
        };
        uint8_t control{};
        if (synth.midi_assigned_to_control(control, param))
            desc.midi_control = control;
        nlohmann::json json;
        to_json(json, desc);
        return json;
    }
}