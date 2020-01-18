
#ifndef GAMMOU_MIDI_DRIVER_H_
#define GAMMOU_MIDI_DRIVER_H_

#include <vector>
#include <functional>
#include <array>

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

                void learn_CC(control_change_callback callback);

            private:
                synthesizer& m_synth;
                control_change_callback m_learning_callback{};
                std::array<control_change_callback, 127> m_controls{};
        };

    }   /*  Sound   */

}   /* Gammou */

#endif