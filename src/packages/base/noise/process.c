
#include <math_utils.h>

struct noise_state
{
    unsigned int state;
};

void node_initialize(struct noise_state *noise)
{
    noise->state = (unsigned int)noise;
}

static float rnd_val(unsigned int rnd)
{
    return ((float)(rnd % 0xFFFFu) - (float)0xFFFFu) / (float)(0x1FFFEu);
}

void node_process(struct noise_state *noise, float *out)
{
    noise->state = linear_congruential_step(noise->state);
    *out = rnd_val(noise->state);
}
