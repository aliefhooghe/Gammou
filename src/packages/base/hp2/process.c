
#include <synthesizer_def.h>
#include <math_utils.h>
#include <math_constant.h>

struct filter_state {
    float input_queue[2];
    float output;
    float output_derivate;
};

void node_initialize(struct filter_state *state)
{
    state->input_queue[0] = 0.f;
    state->input_queue[1] = 0.f;
    state->output = 0.f;
    state->output_derivate = 0.f;
}

static float yd2(float d2x, float omega0, float Q, float y, float dy)
{
    return d2x - omega0 * ((dy / Q) + omega0 * y);
}

void node_process(struct filter_state *state, float in, float cutoff_freq, float q, float *out)
{
    const double omega0 = 6.28318530718f * clamp(cutoff_freq, 1.f, 16000.f);
    const double Q = clamp(q, 0.6f, 100.f);

    const double dt = _sample_duration;
    const double y = state->output;
    const double yd = state->output_derivate;
    const double xd2 = (in - 2.f * state->input_queue[0] + state->input_queue[1]) / (dt * dt);

    const double k1 = yd2(xd2, omega0, Q, y, yd);
    const double k2 = yd2(xd2, omega0, Q, y + (dt / 2.f) * yd, yd + (dt / 2.f) * k1);
    const double k3 = yd2(xd2, omega0, Q, y + (dt / 2.f) * yd + ((dt * dt) / 4.f) * k1, yd + (dt / 2.f) * k2);
    const double k4 = yd2(xd2, omega0, Q, y + dt * yd + ((dt * dt) / 2.f) * k2, yd + dt * k3);

    state->output_derivate += (dt / 6.f) * (k1 + 2.f * k2 + 2.f * k3 + k4);
    state->input_queue[1] = state->input_queue[0];
    state->input_queue[0] = in;

    state->output += dt * yd + ((dt * dt) / 6.f) * (k1 + k2 + k3);
    *out = state->output;
}
