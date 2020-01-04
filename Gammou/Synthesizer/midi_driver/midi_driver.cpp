
#include "midi_driver.h"

#define MIDI_NOTE_OFF	0x80u
#define MIDI_NOTE_ON	0x90u
#define MIDI_AFTERTOUCH	0xA0u

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
                }
                break;

                case MIDI_NOTE_OFF:
                {
                    const uint8_t note = (midi_data[1] & 0x7f);
                    const uint8_t velocity = (midi_data[2] & 0x7f);
                    m_synth.send_note_off(note, static_cast<float>(velocity) / 127.0f);
                }
                break;

                case MIDI_AFTERTOUCH:
                {
                    const uint8_t note = (midi_data[1] & 0x7f);
                    const uint8_t presure = (midi_data[2] & 0x7f);
                    DEBUG_PRINT("Polyphnoic aftertouch [note = %u, presure = %u]\n", note, presure);
                    m_synth.send_aftertouch(note, static_cast<float>(presure) / 127.0f);
                }
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