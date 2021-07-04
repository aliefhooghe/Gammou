
#include <math.h>
#include <synthesizer_def.h>

#include "../common_libs/sample_queue.h"

#define QUEUE_SIZE 48000u

//  state = [read_idx +, write_idx +, read_idx -, write_idx -, data...+, data...+]

struct delay_state {
    int write_idx;
    float data[QUEUE_SIZE];
};

void node_initialize(struct delay_state*state)
{
    state->write_idx = 0;

    for (int i = 0; i < QUEUE_SIZE; ++i)
        state->data[i] = 0.f;
}

void node_process(struct delay_state *state, float in, float g, float t,  float *out)
{
    const float gain = powf(g, t);

    const float fidx = t * _sample_rate;
    const unsigned int idx = fidx;
    const float factor = fidx - (float)idx;

    queue_write_sample(in, &(state->write_idx), state->data, QUEUE_SIZE);

    *out = gain * (
        queue_read_sample(idx, state->write_idx, state->data, QUEUE_SIZE) * (1.f - factor) +
        queue_read_sample(idx + 1, state->write_idx, state->data, QUEUE_SIZE) * factor);
}
