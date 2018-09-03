
#include "midi_driver.h"

#define MIDI_NOTE_OFF	0x80u
#define MIDI_NOTE_ON	0x90u

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

			const uint8_t note =
				(midi_data[1] & 0x7f);

			const uint8_t velocity =
				(midi_data[2] & 0x7f);

            if (cmd == MIDI_NOTE_OFF ||
				(cmd == MIDI_NOTE_ON && velocity == 0u))
				m_synth.send_note_off(note, static_cast<float>(velocity) / 127.0f);
			else if (cmd == MIDI_NOTE_ON)
				m_synth.send_note_on(note, static_cast<float>(velocity) / 127.0f);
            
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