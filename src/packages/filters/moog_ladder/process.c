
#include <math.h>

static float clamp(float x, float min, float max)
{
    if (x < min) return min;
    else if (x > max) return max;
    else return x;
}

void node_process(void *state, float in, float cutoff_freq, float g_res, float *out)
{
    const float dt = 1.0f / 48000.f;
    const float glp1 = M_PI * clamp(cutoff_freq, 1, 18000.0) * dt;

    float *filter_state = (float*)state; // = [lp1, lp1, lp1, lp1, integrator, feedback]

    float x = tanhf(in * (1.f - 2.f * g_res) - 4.f * g_res * filter_state[5]);

    //
    for (int i = 0; i < 4; ++i) {
        x = (filter_state[i] + glp1 * x) / (1.f + glp1);
        filter_state[i] = x * 2.f - filter_state[i];
    }

    filter_state[5] = x;
    *out = x;
}