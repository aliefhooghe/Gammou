
#include <math.h>
#include <synthesizer_def.h>

struct hammer_state {
    float pos;
    float speed;
};

void node_initialize(struct hammer_state *state)
{
    state->pos = -1.f;
    state->speed = 100.f;
}

void node_process(struct hammer_state *state, float in_m, float k, float in_p, float *out_p, float *out_m)
{
    state->pos += state->speed / _sample_rate;

    if (state->pos <= 0.) {     //  no contact
        //  total transmition
        *out_p = in_p;
        *out_m = in_m;
    }
    else {                          //  contact
        //  total reflexion + hammer force
        const float normalized_hammer_force = powf(state->pos, k);
        const float hammer_force = normalized_hammer_force * 50000.0;
        state->speed -= hammer_force / _sample_rate;
        *out_p = -in_m + normalized_hammer_force;
        *out_m = -in_p + normalized_hammer_force;
    }
}
