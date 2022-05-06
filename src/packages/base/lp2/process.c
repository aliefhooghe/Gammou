
#include <synthesizer_def.h>
#include <math_utils.h>
#include <math_constant.h>


struct filter_state {
    // [Xn-1, Xn-2, Xn-3, Yn-1, Yn-2, Yn-3]
    float state[6];
};
void node_initialize(struct filter_state *state)
{
    for (int i = 0; i < 6; ++i)
        state->state[i] = 0.f;
}

void node_process(struct filter_state *state, float in, float cutoff_freq, float q, float *out)
{
    const float dt = _sample_duration;
    const float Q = 0.125f / clamp(q, 0.015625f, 8192.f);
    const float omega = CST_2_PI * clamp(cutoff_freq, 8.f, _sample_rate / 2.f);

    const float a = omega * omega * dt * dt * dt;
    const float tmp = 4.f * omega * Q * dt;
    const float b = dt * (tmp + 4.f);
    const float c = dt * (tmp - 4.f);

    *out =
        (
          a * (state->state[2] + 3.f * (state->state[1] + state->state[0]) + in) -
          (
            (a - c) * state->state[5] +
            (3.f * a - b) * state->state[4] +
            (c + 3.f * a) * state->state[3]))
        / (a + b);

    state->state[5] = state->state[4];
    state->state[4] = state->state[3];
    state->state[3] = *out;
    state->state[2] = state->state[1];
    state->state[1] = state->state[0];
    state->state[0] = in;
}