#include <math_utils.h>

void node_initialize(void *state_ptr)
{
    // [phase1, phase2]
    float *state = (float*)state_ptr;
    state[0] = -0.5f;
    state[1] = 0.5f;
}

/**
 *  \brief 2 DPW (Differential parabolic waveform) sawtooth oscillator are used to generate square waveform
 **/
void node_process(void *state_ptr, float freq, float *out)
{
    const float sample_rate = 48000.f;
    const float dt = 1.f / sample_rate;
    const float fr = clamp(freq, 0.5f, 18000.f);
    const float output_factor = sample_rate / (4.f * fr * (1.f - fr/sample_rate));
    float *state = (float*)state_ptr;

    float phase1 = state[0];
    float phase2 = state[1];
    float next_phase1 = phase1 + fr * dt;
    float next_phase2 = phase2 + fr * dt;
    if (next_phase1 > 1.f) next_phase1 -= 2.f;
    if (next_phase2 > 1.f) next_phase2 -= 2.f;

    const float dpw = phase1 * phase1 - phase2 * phase2;
    const float next_dpw = next_phase1 * next_phase1 - next_phase2 * next_phase2;

    //  output scaled differential dpw
    *out = output_factor * (next_dpw - dpw);

    state[0] = next_phase1;
    state[1] = next_phase2;
}