
#include "synthesizer/synthesizer.h"
#include <nlohmann/json.hpp>

namespace Gammou {

    synthesizer::parameter parameter_from_json(const nlohmann::json& json, synthesizer& synth);
    nlohmann::json parameter_to_json(const synthesizer& synth,const synthesizer::parameter& param);

}