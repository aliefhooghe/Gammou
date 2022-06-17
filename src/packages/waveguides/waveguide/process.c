
#include <math.h>
#include <synthesizer_def.h>

#include "../common_libs/sample_queue.h"

#define QUEUE_SIZE 10000u

struct waveguide_state {
    int write_idx_p;
    int write_idx_m;
    float data_p[QUEUE_SIZE];
    float data_m[QUEUE_SIZE];
    float delay;
};

void node_initialize(struct waveguide_state *state)
{
    state->write_idx_p = 0;
    state->write_idx_m = 0;
    state->delay = _sample_duration;

    for (int i = 0; i < QUEUE_SIZE; ++i)
        state->data_p[i] = 0.f;
    for (int i = 0; i < QUEUE_SIZE; ++i)
        state->data_m[i] = 0.f;
}

void node_push(struct waveguide_state *state, float in_m, float t, float g, float in_p)
{
    const float delay = t > _sample_duration ? t : _sample_duration;
    const float gain = powf(g, delay);
    state->delay = delay;
    queue_write_sample(gain * in_p, &(state->write_idx_p), state->data_p, QUEUE_SIZE);
    queue_write_sample(gain * in_m, &(state->write_idx_m), state->data_m, QUEUE_SIZE);
}

void node_pull(struct waveguide_state *state, float *out_p, float *out_m)
{
    const float fidx = state->delay * _sample_rate - 1.f;
    const unsigned int idx = (unsigned int)fidx;
    const float factor = fidx - (float)idx;

    *out_p = (
        queue_read_sample(idx, state->write_idx_p, state->data_p, QUEUE_SIZE) * (1.f - factor) +
        queue_read_sample(idx + 1, state->write_idx_p, state->data_p, QUEUE_SIZE) * factor);
    *out_m = (
        queue_read_sample(idx, state->write_idx_m, state->data_m, QUEUE_SIZE) * (1.f - factor) +
        queue_read_sample(idx + 1, state->write_idx_m, state->data_m, QUEUE_SIZE) * factor);
}
