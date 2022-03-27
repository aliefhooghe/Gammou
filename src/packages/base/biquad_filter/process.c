
#include <synthesizer_def.h>
#include <math_constant.h>
#include <math_utils.h>

#include <math.h>

struct biquad_state {
    float w[2];
};

void node_initialize(struct biquad_state *state)
{
    state->w[0] = 0.f;
    state->w[1] = 0.f;
}

/**
 *  \brief
 */
void node_process(struct biquad_state *state, float in, float cutoff_freq, float Q, float *out)
{
    const float omega = CST_2_PI * clamp(cutoff_freq, 1.f, _sample_rate / 2.f) / _sample_rate;
    const float cos_omega = cos(omega);
    const float sin_omega = sin(omega);
    const float safeQ = 0.1f + clamp(Q, 0.f, _sample_rate);
    const float alpha = sin_omega / (2.f * safeQ);

    const float a0 = 1.f + alpha;
    const float a1 = (-2.f * cos_omega) / a0;
    const float a2 = (1.f - alpha) / a0;

    const float b1 = (1.f - cos_omega) / a0;
    const float b0 = (b1 / 2.f) / a0;
    const float b2 = (b0) / a0;

    const float w = in - a1 * state->w[0] - a2 * state->w[1];
    *out = b0 * w + b1 * state->w[0] + b2 * state->w[1];

    state->w[1] = state->w[0];
    state->w[0] = w;
}