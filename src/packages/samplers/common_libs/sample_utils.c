
#include "sample_utils.h"

float channel_sample(const wav_channel *sample, int idx)
{
    if (idx < 0 || idx >= sample->sample_count)
        return 0.f;
    else
        return sample->samples[idx];
}

float channel_value_raw(const wav_channel *sample, float t)
{
    const float fidx = t * sample->sample_rate;
    return channel_sample(sample, (int)(t * _sample_rate));
}


float channel_value_lin(const wav_channel *sample, float t)
{
    const float fidx = t * sample->sample_rate;
    const int idx = (int)fidx;
    const float factor = fidx - (float)idx;
    return factor * channel_sample(sample, idx) + (1.f - factor) * channel_sample(sample, idx);
}