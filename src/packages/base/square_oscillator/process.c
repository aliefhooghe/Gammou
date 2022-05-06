#include <synthesizer_def.h>
#include <math_utils.h>

struct square_state
{
    float phase1;
    float phase2;
};


void node_initialize(struct square_state *state)
{
    // [phase1, phase2]
    state->phase1 = -0.5f;
    state->phase2 = 0.5f;
}

/**
 *  \brief 2 DPW (Differential parabolic waveform) sawtooth oscillator are used to generate square waveform
 **/
void node_process(struct square_state *state, float freq, float *out)
{
    const float dt = _sample_duration;
    const float fr = clamp(freq, 0.5f, 18000.f);
    const float output_factor = _sample_rate / (4.f * fr * (1.f - fr/ _sample_rate));

    float next_phase1 = state->phase1 + fr * dt;
    float next_phase2 = state->phase2 + fr * dt;
    if (next_phase1 > 1.f) next_phase1 -= 2.f;
    if (next_phase2 > 1.f) next_phase2 -= 2.f;

    const float dpw = state->phase1 * state->phase1 - state->phase2 * state->phase2;
    const float next_dpw = next_phase1 * next_phase1 - next_phase2 * next_phase2;

    //  output scaled differential dpw
    *out = output_factor * (next_dpw - dpw);

    state->phase1 = next_phase1;
    state->phase2 = next_phase2;
}