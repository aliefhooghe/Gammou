
#include <math.h>

#include <synthesizer_def.h>
#include <math_constant.h>
#include <math_utils.h>

struct sin_state
{
    float phase;
};

void node_initialize(struct sin_state *state)
{
    state->phase = 0.f;
}

void node_process(struct sin_state *state, float freq, float *out)
{
    const float limit = _sample_rate / 2.f;
    *out = sin(state->phase);
    state->phase += clamp(freq, -limit, limit) / _sample_rate * CST_2_PI;
    if (state->phase > CST_PI)
        state->phase -= CST_2_PI;
    else if (state->phase < -CST_PI)
        state->phase += CST_2_PI;
}