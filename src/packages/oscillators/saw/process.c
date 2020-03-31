

/**
 *  \brief A DPW (Differential parabolic waveform) sawtooth oscillator
 **/
void node_process(void *state_ptr, float freq, float *out)
{
    const float sample_rate = 48000.f;
    const float dt = 1.0f / sample_rate;
    const float output_factor = sample_rate / (4.f * freq * (1.f - freq/sample_rate));
    float *state = (float*)state_ptr;

    float phase = state[0];
    float dpw = state[1];

    const float current_dpw = phase * phase;

    //  output scaled differential dpw
    *out = output_factor * (current_dpw - dpw);

    //  phase is the naive saw
    phase += freq * dt;
    if (phase > 1.f)
        phase = -1.f;

    state[0] = phase;
    state[1] = current_dpw;
}