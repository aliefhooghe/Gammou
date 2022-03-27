
#include <synthesizer_def.h>

struct ramp_state {
    float value;
};

void node_initialize(struct ramp_state* state)
{
    state->value = 0.f;
}

void node_process(struct ramp_state* state, float speed, float *out)
{
    state->value += speed / _sample_rate;
    *out = state->value;
}
