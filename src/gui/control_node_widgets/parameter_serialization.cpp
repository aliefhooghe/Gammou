
#include <optional>
#include <stdint.h>

#include "parameter_serialization.h"

namespace Gammou {

    struct parameter_descriptor
    {
        float normalized;
        float shape_base;
        float shape_scale;
        std::optional<uint8_t> control;
    };

    synthesizer::parameter parameter_from_json(const nlohmann::json& json, synthesizer& synth)
    {
        float normalized{};
        float shape_base{};
        float shape_scale{};

        json.at("normalized").get_to(normalized);
        json.at("shape-base").get_to(shape_base);
        json.at("shape-scale").get_to(shape_scale);

        auto param = synth.allocate_parameter(normalized);
        param.set_shape_base(shape_base);
        param.set_shape_scale(shape_scale);

        auto it = json.find("control");
        if (it != json.end())
            synth.midi_assign_control(it->get<uint8_t>(), param);

        return param;
    }

    nlohmann::json parameter_to_json(const synthesizer& synth,const synthesizer::parameter& param)
    {
        nlohmann::json json{
            {"normalized", param.get_normalized()},
            {"shape-base", param.get_shape_base()},
            {"shape-scale", param.get_shape_scale()}
        };

        uint8_t control;
        if (synth.midi_assigned_to_control(control, param))
            json["control"] = control;

        return json;
    }
}