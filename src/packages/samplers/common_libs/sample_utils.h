#ifndef GAMMOU_SAMPLE_UTILS_H_
#define GAMMOU_SAMPLE_UTILS_H_

#include <synthesizer_def.h>

float channel_sample(const wav_channel *sample, int idx);
float channel_value_raw(const wav_channel *sample, float t);
float channel_value_lin(const wav_channel *sample, float t);

#endif