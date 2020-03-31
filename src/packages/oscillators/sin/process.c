
#include <math.h>

void node_process(void *state, float freq, float p, float *out)
{
    float *phase = (float*)state;
    *out = sin(*phase + p);

    *phase += (freq / 48000.f) * 2.f * M_PI;
    if (*phase > 2.f * M_PI + 0)
        *phase -= (2.f * M_PI);
}