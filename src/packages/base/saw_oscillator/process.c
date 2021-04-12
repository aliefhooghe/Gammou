
#include <synthesizer_def.h>

#include "../common_libs/math_utils.h"

void node_initialize(float *state)
{
    *state = 0.f;
}

/**
 *  \brief A DPW (Differential parabolic waveform) sawtooth oscillator
 **/
void node_process(float *state, float freq, float *out)
{
    const float dt = 1.f / _sample_rate;
    const float fr = clamp(freq, 0.5f, 18000.f);
    const float output_factor = _sample_rate / (4.f * fr * (1.f - fr / _sample_rate));

    float phase = state[0];
    float next_phase = phase + fr * dt;
    if (next_phase > 1.f)
        next_phase -= 2.f;


    const float dpw = phase * phase;
    const float next_dpw = next_phase * next_phase;

    //  output scaled differential dpw
    *out = output_factor * (next_dpw - dpw);

    state[0] = next_phase;
}