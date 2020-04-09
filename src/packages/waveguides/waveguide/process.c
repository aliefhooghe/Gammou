
#include <math.h>
#include "../common_libs/sample_queue.h"

#define QUEUE_SIZE 10000u

//  state = [read_idx +, write_idx +, read_idx -, write_idx -, data...+, data...+]

struct state_header {
    int write_idx_p;
    int write_idx_m;
};

void node_initialize(void *state)
{
    struct state_header* header = (struct state_header*)state;
    float *data_p = (float*)(sizeof(struct state_header) + (char*)state);
    float *data_m = data_p + QUEUE_SIZE;

    header->write_idx_p = 0;
    header->write_idx_m = 0;

    for (int i = 0; i < QUEUE_SIZE; ++i)
        data_p[i] = 0.f;
    for (int i = 0; i < QUEUE_SIZE; ++i)
        data_m[i] = 0.f;
}

void node_process(void *state, float in_m, float t, float g, float in_p, float *out_p, float *out_m)
{
    struct state_header* header = (struct state_header*)state;
    float *data_p = (float*)(sizeof(struct state_header) + (char*)state);
    float *data_m = data_p + QUEUE_SIZE;

    const float gain = powf(g, t);

    const float fidx = t * 48000.f - 1.f;
    const unsigned int idx = fidx;
    const float factor = fidx - (float)idx;

    queue_write_sample(in_p, &(header->write_idx_p), data_p, QUEUE_SIZE);
    queue_write_sample(in_m, &(header->write_idx_m), data_m, QUEUE_SIZE);

    *out_p = gain * (
        queue_read_sample(idx, header->write_idx_p, data_p, QUEUE_SIZE) * (1.f - factor) +
        queue_read_sample(idx + 1, header->write_idx_p, data_p, QUEUE_SIZE) * factor);
    *out_m = gain * (
        queue_read_sample(idx, header->write_idx_m, data_m, QUEUE_SIZE) * (1.f - factor) +
        queue_read_sample(idx + 1, header->write_idx_m, data_m, QUEUE_SIZE) * factor);
}
