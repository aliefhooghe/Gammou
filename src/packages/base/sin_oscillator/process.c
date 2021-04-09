
#include "../common_libs/math_constant.h"
#include "../common_libs/math_utils.h"

void node_initialize(float *phase)
{
    *phase = 0.f;
}

void node_process(float *phase, float freq, float *out)
{
    *out = fast_sin(*phase);
    *phase += clamp(freq, -24000.0, 24000.0) / 48000.f * CST_2_PI;
    if (*phase > CST_PI)
        *phase -= CST_2_PI;
    else if (*phase < -CST_PI)
        *phase += CST_2_PI;
}