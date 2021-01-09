
#include <math.h>
#include "../common_libs/sample_queue.h"

#define QUEUE_SIZE 10000u

//  state = [read_idx +, write_idx +, read_idx -, write_idx -, data...+, data...+]

struct waveguide_state {
    int write_idx_p;
    int write_idx_m;
    float data_p[QUEUE_SIZE];
    float data_m[QUEUE_SIZE];
};

void node_initialize(struct waveguide_state *state)
{
    state->write_idx_p = 0;
    state->write_idx_m = 0;

    for (int i = 0; i < QUEUE_SIZE; ++i)
        state->data_p[i] = 0.f;
    for (int i = 0; i < QUEUE_SIZE; ++i)
        state->data_m[i] = 0.f;
}

void node_process(struct waveguide_state *state, float in_m, float t, float g, float in_p, float *out_p, float *out_m)
{
    const float gain = powf(g, t);

    const float fidx = t * 48000.f - 1.f;
    const unsigned int idx = fidx;
    const float factor = fidx - (float)idx;

    queue_write_sample(in_p, &(state->write_idx_p), state->data_p, QUEUE_SIZE);
    queue_write_sample(in_m, &(state->write_idx_m), state->data_m, QUEUE_SIZE);

    *out_p = gain * (
        queue_read_sample(idx, state->write_idx_p, state->data_p, QUEUE_SIZE) * (1.f - factor) +
        queue_read_sample(idx + 1, state->write_idx_p, state->data_p, QUEUE_SIZE) * factor);
    *out_m = gain * (
        queue_read_sample(idx, state->write_idx_m, state->data_m, QUEUE_SIZE) * (1.f - factor) +
        queue_read_sample(idx + 1, state->write_idx_m, state->data_m, QUEUE_SIZE) * factor);
}
