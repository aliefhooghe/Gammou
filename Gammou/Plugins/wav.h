#ifndef WAV_H
#define WAV_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct wav_t wav_t;

wav_t *wav_copy(const wav_t *src);
int wav_save(wav_t *wav, const char *path, const unsigned bit_depth);

wav_t *wav_load(const char *path);
int wav_free(wav_t *wav);

unsigned int wav_get_sample_count(const wav_t *wav);
unsigned int wav_get_channel_count(const wav_t *wav);
unsigned int wav_get_samplerate(const wav_t *wav);
double wav_get_sample(
  wav_t *wav,
  const unsigned int pos, 
  const unsigned int channel);

void wav_set_sample(
  wav_t *wav,
  const double sample, 
  const unsigned int pos, 
  const unsigned int channel);

#ifdef __cplusplus
}
#endif

#endif


