#ifndef GAMMOU_SYNTHESIZER_DEF_H_
#define GAMMOU_SYNTHESIZER_DEF_H_

#include "static_memory_chunk_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief Global synthesizer variables
 */
extern const float _sample_rate;        // The synthesizer sample rate (Hz)
extern const float _sample_duration;    // The synthesizer sample duration (Sec)

#ifdef __cplusplus
}
#endif

#endif