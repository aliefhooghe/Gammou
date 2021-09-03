
#include <synthesizer_def.h>
#include <math_utils.h>

struct filter_state {
    // = [lp_0, lp_0, lp_1, lp_1, lp_2, lp_2, lp_3, lp_3, feedback]
    float state[9];
};

void node_initialize(struct filter_state *state)
{
    for (int i = 0; i < 9; ++i)
        state->state[i] = 0.f;
}

void node_process(struct filter_state *state, float in, float cutoff_freq, float g_res, float *out)
{
    const float omega_c = 6.28318530718f * clamp(cutoff_freq, 1, _sample_rate / 2.f) / _sample_rate;
    const float glp1 = omega_c * (0.9892f - omega_c * (0.4342f - omega_c * (0.1381f - 0.0202f * omega_c)));
    const float safe_gres = clamp(g_res, 0.f, 20.f);
    float x = fast_tanh(in * (1.f - 2.f * safe_gres) - 4.f * safe_gres * state->state[8]);

    for (int i = 0; i < 4; ++i) {
        float tmp = 0.7692307692307692f * x + 0.23076923076923075f * state->state[2 * i];
        state->state[2 * i] = x;
        x = state->state[2 * i + 1] + glp1 * (tmp - state->state[2 * i + 1]);
        state->state[2 * i + 1] = x;
    }

    state->state[8] = x;
    *out = x;
}