
#include <synthesizer_def.h>

#include "../common_libs/math_constant.h"
#include "../common_libs/math_utils.h"

void node_initialize(float *phase)
{
    *phase = 0.f;
}

void node_process(float *phase, float freq, float *out)
{
    const float limit = _sample_rate / 2.f;
    *out = fast_sin(*phase);
    *phase += clamp(freq, -limit, limit) / _sample_rate * CST_2_PI;
    if (*phase > CST_PI)
        *phase -= CST_2_PI;
    else if (*phase < -CST_PI)
        *phase += CST_2_PI;
}