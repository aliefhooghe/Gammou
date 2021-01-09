
#include "../common_libs/math_constant.h"
#include "../common_libs/math_utils.h"

void node_initialize(float *phase)
{
    *phase = 0.f;
}

void node_process(float *phase, float freq, float p, float *out)
{
    *out = fast_sin(*phase);

    *phase += clamp((freq / 48000.f) * PI * 2.f + p, -PI, PI);
    if (*phase > PI)
        *phase -= PI * 2.f;
}