
#include "../common_libs/math_constant.h"

void node_initialize(float *state)
{
    *state = 0.f;
}

/**
 *  \brief A first order low pass filter
 *  \param state filter state : a floating point number
 *  \param in one input sample
 *  \param cutoff_freq filter cutoff frequency in Hz
 *  \param out one filter output sample
 */
void node_process(float *state, float in, float cutoff_freq, float *out)
{
    const float dt = 1.0f / 48000.f;
    const float g = PI * cutoff_freq * dt;
    *out = (*state + g * in) / (1.f + g);
    *state = *out * 2.f - *state;
}