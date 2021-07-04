
#include <synthesizer_def.h>
#include <math_constant.h>

struct lp1_state
{
    float state;
};

void node_initialize(struct lp1_state *lp1)
{
    lp1->state = 0.f;
}

/**
 *  \brief A first order low pass filter
 *  \param state filter state : a floating point number
 *  \param in one input sample
 *  \param cutoff_freq filter cutoff frequency in Hz
 *  \param out one filter output sample
 */
void node_process(struct lp1_state *lp1, float in, float cutoff_freq, float *out)
{
    const float dt = 1.0f / _sample_rate;
    const float g = CST_PI * cutoff_freq * dt;
    *out = (lp1->state + g * in) / (1.f + g);
    lp1->state = *out * 2.f - lp1->state;
}