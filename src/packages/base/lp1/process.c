
#include "../common_libs/math_constant.h"

void node_initialize(void *state)
{
    float *filter_state = (float*)state;
    *filter_state = 0.f;
}

/**
 *  \brief A first order low pass filter
 *  \param state filter state : a floating point number
 *  \param in one input sample
 *  \param cutoff_freq filter cutoff frequency in Hz
 *  \param out one filter output sample
 */
void node_process(void *state, float in, float cutoff_freq, float *out)
{
    float *filter_state = (float*)state;
    const float dt = 1.0f / 48000.f;
    const float g = PI * cutoff_freq * dt;
    *out = (*filter_state + g * in) / (1.f + g);
    *filter_state = *out * 2.f - *filter_state;
}