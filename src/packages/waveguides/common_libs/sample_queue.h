#ifndef GAMMOU_WAVEGUIDES_SAMPLE_QUEUE_H_
#define GAMMOU_WAVEGUIDES_SAMPLE_QUEUE_H_

float queue_read_sample(
    int idx,
    int write_ptr,
    const float *data,
    const int size);

void queue_write_sample(
    float sample,
    int *write_ptr,
    float *data,
    const int size);

#endif