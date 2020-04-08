
static float clamp(float x, float min, float max)
{
    if (x < min) return min;
    else if (x > max) return max;
    else return x;
}

static float my_than(float x)
{
    //  Henry lambert formula
    const float x2 = x*x;
    return ((((x2 + 378.f) * x2 + 17325.f) * x2 + 135135.f) * x) /
            (((28.f * x2 + 3150.f) * x2 + 62370.f) * x2 + 135135.f);
}

void node_initialize(void *state)
{
    float *filter_state = (float*)state;
    for (int i = 0; i < 9; ++i)
        filter_state[i] = 0.f;
}

void node_process(void *state, float in, float cutoff_freq, float g_res, float *out)
{
    const float omega_c = 6.28318530718f * clamp(cutoff_freq, 1, 18000.0) / 48000.f;
    const float glp1 = omega_c * (0.9892f - omega_c * (0.4342f - omega_c * (0.1381f - 0.0202f * omega_c)));

    float *filter_state = (float*)state; // = [lp_0, lp_0, lp_1, lp_1, lp_2, lp_2, lp_3, lp_3, feedback]

    float x = my_than(in * (1.f - 2.f * g_res) - 4.f * g_res * filter_state[8]);

    for (int i = 0; i < 4; ++i) {
        float tmp = 0.7692307692307692f * x + 0.23076923076923075f * filter_state[2 * i];
        filter_state[2 * i] = x;
        x = filter_state[2 * i + 1] + glp1 * (tmp - filter_state[2 * i + 1]);
        filter_state[2 * i + 1] = x;
    }

    filter_state[8] = x;
    *out = x;
}