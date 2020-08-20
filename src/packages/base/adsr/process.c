
struct adsr_state
{
    float out;
    enum {ATTACK, DECAY, RELEASE} state;
};


//  state = [command, out]
void node_initialize(void *state)
{
    struct adsr_state *adsr_state = (struct adsr_state*)state;
    adsr_state->out = 0u;
    adsr_state->state = ATTACK;
}

void node_process(
    void *state,
    float gate,
    float attack, float decay, float sustain, float release,
    float *out)
{
    struct adsr_state *adsr_state = (struct adsr_state*)state;
    float characteristic_time, command;

    if (gate > 0.5f) {
        if (adsr_state->state == RELEASE)
            adsr_state->state = ATTACK;

        if (adsr_state->state == ATTACK) {
            characteristic_time = attack;
            command = 1.f;
            if (adsr_state->out > 0.9f)
                adsr_state->state = DECAY;
        }
        else {
            characteristic_time = decay;
            command = sustain;
        }
    }
    else {
        adsr_state->state = RELEASE;
        characteristic_time = release;
        command = 0.f;
    }

    const float factor = characteristic_time * 48000.0;
    *out = (command + factor * adsr_state->out) / (1.f + factor);
    adsr_state->out = *out;
}