
#include <math.h>

// state = [hammer pos, hammer speed]

void node_initialize(void *state)
{
    float *hammer_state = (float*)state;
    hammer_state[0] = -1.f;
    hammer_state[1] = 100.f;
}

void node_process(void *state, float in_m, float k, float in_p, float *out_p, float *out_m)
{
    const float dt = 1.f / 48000.f;
    float *hammer_state = (float*)state;

    hammer_state[0] += dt * hammer_state[1];

    if (hammer_state[0] <= 0.) {     //  no contact
        //  total transmition
        *out_p = in_p;
        *out_m = in_m;
    }
    else {                          //  contact
        //  total reflexion + hammer force
        const float normalized_hammer_force = powf(hammer_state[0], k);
        const float hammer_force = normalized_hammer_force * 50000.0;
        hammer_state[1] -= dt * hammer_force;
        *out_p = -in_m + normalized_hammer_force;
        *out_m = -in_p + normalized_hammer_force;
    }
}
