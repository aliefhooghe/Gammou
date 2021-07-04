#ifndef STATIC_MEMORY_CHUNK_TYPES_H_
#define STATIC_MEMORY_CHUNK_TYPES_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
    unsigned int sample_count;
    float sample_rate;
    float samples[1];
} wav_channel;

// typedef struct
// {
//     float samples[1]
// } wav_sample;

#ifdef __cplusplus
}
#endif

#endif