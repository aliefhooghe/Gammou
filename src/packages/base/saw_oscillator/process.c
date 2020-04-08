

#include "../common_libs/math_utils.h"

void node_initialize(void *state_ptr)
{
    float *state = (float*)state_ptr;
    state[0] = 0.f;
    state[1] = 0.f;
}

/**
 *  \brief A DPW (Differential parabolic waveform) sawtooth oscillator
 **/
void node_process(void *state_ptr, float freq, float *out)
{
    const float sample_rate = 48000.f;
    const float dt = 1.f / sample_rate;
    const float fr = clamp(freq, 0.5f, 18000.f);
    const float output_factor = sample_rate / (4.f * fr * (1.f - fr/sample_rate));
    float *state = (float*)state_ptr;

    float phase = state[0];
    float dpw = state[1];

    const float current_dpw = phase * phase;

    //  output scaled differential dpw
    *out = output_factor * (current_dpw - dpw);

    //  phase is the naive saw
    phase += fr * dt;
    if (phase > 1.f)
        phase -= 2.f;

    state[0] = phase;
    state[1] = current_dpw;
}