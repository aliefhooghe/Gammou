
#include <synthesizer_def.h>

struct adsr_state
{
    float out;
    enum {ATTACK, DECAY, RELEASE} state;
};


//  state = [command, out]
void node_initialize(struct adsr_state *state)
{
    state->out = 0u;
    state->state = ATTACK;
}

void node_process(
    struct adsr_state *state,
    float gate,
    float attack, float decay, float sustain, float release,
    float *out)
{
    float characteristic_time, command;

    if (gate > 0.5f) {
        if (state->state == RELEASE)
            state->state = ATTACK;

        if (state->state == ATTACK) {
            characteristic_time = attack;
            command = 1.f;
            if (state->out > 0.9f)
                state->state = DECAY;
        }
        else {
            characteristic_time = decay;
            command = sustain;
        }
    }
    else {
        state->state = RELEASE;
        characteristic_time = release;
        command = 0.f;
    }

    const float factor = characteristic_time * _sample_rate;
    *out = (command + factor * state->out) / (1.f + factor);
    state->out = *out;
}