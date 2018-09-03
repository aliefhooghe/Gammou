
#include <vector>
#include "synthesizer.h"

namespace Gammou {

    namespace Sound {

        class midi_driver {

            public:
                midi_driver(synthesizer& synth);

                void handle_midi_event(const void *data);
                void handle_midi_event(const std::vector<char>& data);
                void handle_midi_event(const std::vector<unsigned char>& data);

            private:
                synthesizer& m_synth;
        };

    }   /*  Sound   */

}   /* Gammou */