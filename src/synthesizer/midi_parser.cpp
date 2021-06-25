
#include "midi_parser.h"

namespace Gammou {

    enum midi_opcode : uint8_t {
        note_off = 0x8u,
        note_on = 0x9u,
        control_change = 0xBu,
        pitch_bend = 0xEu

        //  to be continued
    };

    void execute_midi_msg(
        synthesizer& synth,
        const uint8_t *data,
        const std::size_t /*size*/)
    {
        const auto status_byte = data[0];
        const auto midi_channel = status_byte & 0xF;
        const auto midi_opcode = (status_byte & 0xF0) >> 4;

        switch (midi_opcode)
        {

        case midi_opcode::note_on:
        {
            const auto note = data[1] & 0x7F;
            const auto velocity = data[2] & 0x7F;

            if (velocity == 0u)
                synth.midi_note_off(note, 0.5f);
            else
                synth.midi_note_on(note, static_cast<float>(velocity) / 127.f);
        }
        break;

        case midi_opcode::note_off:
        {
            const auto note = data[1] & 0x7F;
            const auto velocity = data[2] & 0x7F;
            synth.midi_note_off(note, static_cast<float>(velocity) / 127.f);
        }
        break;

        case midi_opcode::control_change:
        {
            const auto control = data[1] & 0x7F;
            const auto value = data[2] & 0x7F;
            synth.midi_control_change(control, static_cast<float>(value) / 127.f);
        }
        break;

        case midi_opcode::pitch_bend:
        {
            const auto value = static_cast<int>(data[2] & 0x7F) - 64;
            synth.midi_control_change(0x80, static_cast<float>(value) / 63.f);
        }
        break;

        default:
            break;
        }
    }

}