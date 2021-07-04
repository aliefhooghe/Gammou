#ifndef GAMMOU_MIDI_PARSER_H_
#define GAMMOU_MIDI_PARSER_H_

#include <cstdint>
#include "synthesizer.h"

namespace Gammou
{
    void execute_midi_msg(
        synthesizer& synth,
        const uint8_t *data,
        const std::size_t size);
}


#endif