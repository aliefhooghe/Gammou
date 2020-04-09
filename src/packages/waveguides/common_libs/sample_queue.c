
#include "sample_queue.h"

float queue_read_sample(
    int idx,
    int write_ptr,
    const float *data,
    const int size)
{
    const int pos = (write_ptr - idx) % size;

    if (pos < 0)
        return data[pos + size];
    else
        return data[pos];
}

void queue_write_sample(
    float sample,
    int *write_ptr,
    float *data,
    const int size)
{
    *write_ptr = (*write_ptr + 1) % size;
    data[*write_ptr] = sample;
}