
#include "midi_driver.h"

#define MIDI_NOTE_OFF               0x80u
#define MIDI_NOTE_ON                0x90u
#define MIDI_POLYPONIC_AFTERTOUCH   0xA0u
#define MIDI_CHANNEL_AFTERTOUCH     0xD0u
#define MIDI_PITCH_BEND             0xE0

namespace Gammou {

    namespace Sound {

        midi_driver::midi_driver(synthesizer& synth)
        :   m_synth(synth)
        {
        }

        void midi_driver::handle_midi_event(const void *data)
        {
            const uint8_t *midi_data = (uint8_t*)data;

            // ignore midi Channels
			const uint8_t cmd =
				(midi_data[0] & 0xf0);


            switch (cmd) {

                case MIDI_NOTE_ON:
                {
                    const uint8_t note = (midi_data[1] & 0x7f);
                    const uint8_t velocity = (midi_data[2] & 0x7f);

                    //  Note On event with velocity == 0 mean note off !
                    if (velocity > 0)
                        m_synth.send_note_on(note, static_cast<float>(velocity) / 127.0f);
                    else
                        m_synth.send_note_off(note, 0.f);
                    DEBUG_PRINT("[MIDI DRIVER] Note On(note = %u, velocity = %u)\n", note, velocity);
                }
                break;

                case MIDI_NOTE_OFF:
                {
                    const uint8_t note = (midi_data[1] & 0x7f);
                    const uint8_t velocity = (midi_data[2] & 0x7f);
                    m_synth.send_note_off(note, static_cast<float>(velocity) / 127.0f);
                    DEBUG_PRINT("[MIDI DRIVER] Note Off(note = %u, velocity = %u)\n", note, velocity);
                }
                break;

                case MIDI_POLYPONIC_AFTERTOUCH:
                {
                    const uint8_t note = (midi_data[1] & 0x7f);
                    const uint8_t presure = (midi_data[2] & 0x7f);
                    DEBUG_PRINT("[MIDI DRIVER] Polyphonic Aftertouch (note = %u, presure = %u)\n", note, presure);
                    m_synth.send_aftertouch(note, static_cast<float>(presure) / 127.0f);
                }
                break;

                case MIDI_CHANNEL_AFTERTOUCH:
                {
                    const uint8_t presure = (midi_data[1] & 0x7f);
                    DEBUG_PRINT("[MIDI DRIVER] Channel Aftertouch (presure = %u)\n", presure);
                }
                break;

                case MIDI_PITCH_BEND:
                {
                    const int16_t value =
                        static_cast<int16_t>((midi_data[1] & 0x7f) | (midi_data[2] << 7)) - 8192;
                    DEBUG_PRINT("[MIDI DRIVER] Pitch Bend ([%d])\n", value);
                    m_synth.send_pitch_bend(static_cast<float>(value) / 8192.0f);
                }
                break;

                default:
                    DEBUG_PRINT("[MIDI DRIVER] Unknown midi cmd : %x\n", cmd);
                break;
            }
        }

        void midi_driver::handle_midi_event(const std::vector<char>& data)
        {
            handle_midi_event(data.data());
        }

        void midi_driver::handle_midi_event(const std::vector<unsigned char>& data)
        {
            handle_midi_event(data.data());
        }

    }   /* Sound */

}   /* Gammou */