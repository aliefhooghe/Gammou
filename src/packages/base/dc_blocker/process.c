
void node_initialize(void *state)
{
    float *filter_state = (float*)state;
    filter_state[0] = 0.f;
    filter_state[1] = 0.f;
}

void node_process(void *state, float in, float *out)
{
    float *filter_state = (float*)state; // [last in, last out]
    const float R = 0.995f;
    *out = in - filter_state[0] + R * filter_state[1];
    filter_state[0] = in;
    filter_state[1] = *out;
}