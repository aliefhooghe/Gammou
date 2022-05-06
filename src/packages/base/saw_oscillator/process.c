
#include <synthesizer_def.h>
#include <math_utils.h>

struct saw_state
{
    float phase;
};

void node_initialize(struct saw_state *saw)
{
    saw->phase = 0.f;
}

/**
 *  \brief A DPW (Differential parabolic waveform) sawtooth oscillator
 **/
void node_process(struct saw_state *saw, float freq, float *out)
{
    const float dt = _sample_duration;
    const float fr = clamp(freq, 0.5f, 18000.f);
    const float output_factor = _sample_rate / (4.f * fr * (1.f - fr / _sample_rate));

    float next_phase = saw->phase + fr * dt;
    if (next_phase > 1.f)
        next_phase -= 2.f;

    const float dpw = saw->phase * saw->phase;
    const float next_dpw = next_phase * next_phase;

    //  output scaled differential dpw
    *out = output_factor * (next_dpw - dpw);
    saw->phase = next_phase;
}