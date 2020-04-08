
#include "../common_libs/math_constant.h"
#include "../common_libs/math_utils.h"

void node_initialize(void *state)
{
    float *phase = (float*)state;
    *phase = 0.f;
}

void node_process(void *state, float freq, float p, float *out)
{
    float *phase = (float*)state;
    *out = fast_sin(*phase + p);

    *phase += (freq / 48000.f) * PI * 2.f;
    if (*phase > PI)
        *phase -= PI * 2.f;
}