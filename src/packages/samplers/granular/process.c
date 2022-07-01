
#include <math.h>

#include <math_utils.h>
#include <sample_utils.h>
#include <synthesizer_def.h>

#define GRAIN_COUNT 16u

struct grain
{
    float grain_cursor; //  in sample
    float grain_center; //  in sample
};

typedef struct
{
   struct grain grains[GRAIN_COUNT];
    float time;
   float first_grain_time;
   unsigned int rnd;

} granular_state;

static float rnd_val(unsigned int rnd)
{
    return ((float)(rnd % 0xFFFFu) - (float)0xFFFFu) / (float)(0x1FFFEu);
}

static float rnd_float(granular_state *state, float center, float width)
{
    const float val = center + rnd_val(state->rnd) * width;
    state->rnd = linear_congruential_step(state->rnd);
    return val;
}

void node_initialize(const wav_channel *sample, granular_state *state)
{
    state->rnd = (unsigned int)state;
    state->time = 0.f;
    state->first_grain_time = -0.5f;

    const float radius = 0.3f;
    for (unsigned int i = 0u; i < GRAIN_COUNT; i++) {
        state->grains[i].grain_center = -1000.f;
        state->grains[i].grain_cursor = -1.f;
    }
}

static float grain_env(float t, float dev)
{
    return expf(-t * t / (2.f * dev * dev));
}

void node_process(const wav_channel *sample, granular_state *state, float pos, float width, float radius, float radius_width, float dev, float speed, float *out)
{
    const float dt = _sample_duration;
    const float max_env_cursor = dev * 4.5f;
    float output = 0.f;

    for (unsigned int i = 0u; i < GRAIN_COUNT; i++)
    {
        const float env_cursor = state->grains[i].grain_cursor - state->grains[i].grain_center;

        if (env_cursor > max_env_cursor)
        {
            state->first_grain_time += (rnd_float(state, radius, radius_width) / speed);
            state->grains[i].grain_center = rnd_float(state, pos, width);
            state->grains[i].grain_cursor = state->grains[i].grain_center + (state->time - state->first_grain_time) * speed;
        }

        const float updated_env_cursor = state->grains[i].grain_cursor - state->grains[i].grain_center;
        output += channel_value_lin(sample, state->grains[i].grain_cursor) * grain_env(env_cursor, dev);
        state->grains[i].grain_cursor += dt * speed;
    }

    state->time += dt;
    *out = output;
}
