
struct filter_state {
    float last_in;
    float last_out;
};

void node_initialize(struct filter_state *state)
{
    state->last_in = 0.f;
    state->last_out = 0.f;
}

void node_process(struct filter_state *state, float in, float *out)
{
    // [last in, last out]
    const float R = 0.995f;
    *out = in - state->last_in + R * state->last_out;
    state->last_in = in;
    state->last_out = *out;
}