
#include <synthesizer_def.h>

typedef struct
{
    float pos;
} sampler_state;

void node_initialize(const wav_channel *sample, sampler_state *state)
{
    state->pos = 0.f;
}

void node_process(const wav_channel *sample, sampler_state *state, float pos, float speed, float *out)
{
    const int idx = (int)(pos * _sample_rate + state->pos);
    *out = sample->samples[idx % sample->sample_count];
    state->pos += speed;
}
