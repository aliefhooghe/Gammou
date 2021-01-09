
#include "../common_libs/math_utils.h"
#include "../common_libs/math_constant.h"


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
    const float dt = 1.f / 48000.f;
    const float Q = q / 128.f;
    const float omega = 2.f * PI * clamp(cutoff_freq, 1.f, 18000.f);

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