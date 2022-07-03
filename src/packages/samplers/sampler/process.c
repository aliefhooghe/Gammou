
#include <synthesizer_def.h>
#include <sample_utils.h>

typedef struct
{
    float pos;
} sampler_state;

void node_initialize(const wav_channel *sample, sampler_state *state)
{
    state->pos = 0.f;
}

void node_process(const wav_channel *const sample, sampler_state *state, float pos, float speed, float *out, float *out2)
{
    *out = channel_value_lin(sample, pos + state->pos);
    *out2 = channel_value_raw(sample, pos + state->pos);
    state->pos += (speed * _sample_duration);
}
