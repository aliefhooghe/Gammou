
#ifndef GAMMOU_MIDI_DRIVER_H_
#define GAMMOU_MIDI_DRIVER_H_

#include <functional>
#include <array>
#include <vector>
#include <map>

#include "synthesizer.h"

namespace Gammou {

    namespace Sound {

        class midi_driver {

            public:
                using control_change_callback = std::function<void(double)>;

                midi_driver(synthesizer& synth);

                void handle_midi_event(const void *data);
                void handle_midi_event(const std::vector<char>& data);
                void handle_midi_event(const std::vector<unsigned char>& data);

                void learn_CC(control_change_callback callback, intptr_t ref);
                void unmap_CC(intptr_t ref);

            private:
                synthesizer& m_synth;

                control_change_callback m_learning_callback{};
                intptr_t m_learning_ref{0u};

                std::array<control_change_callback, 127> m_controls{};
                std::map<intptr_t, uint8_t> m_controls_ref{};
        };

    }   /*  Sound   */

}   /* Gammou */

#endif