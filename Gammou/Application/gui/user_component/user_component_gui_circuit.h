#ifndef USER_COMPONENT_GUI_CIRCUIT_H_
#define USER_COMPONENT_GUI_CIRCUIT_H_

#include <gui_synthesizer_circuit.h>

namespace Gammou {

    namespace Gui {

        class user_sound_component;

        class user_component_gui_circuit :
            public abstract_gui_circuit
        {
            friend class user_sound_component;

            enum internal_component_id : uint32_t
            {
                    INPUT = 0u,
                    OUTPUT
            };

            public:
                ~user_component_gui_circuit();

            private:
                user_component_gui_circuit(
                    gui_component_main_factory& factory,
                    user_sound_component& component,
                    const int x,
                    const int y,
                    const unsigned int width,
                    const unsigned int height,
                    const View::color background = GuiProperties::background);

                void add_sound_component_to_frame(Sound::abstract_sound_component *sound_component) override;
                abstract_gui_component *gui_component_by_internal_id(const uint32_t id) override;

                void lock_circuit() override {}
                void unlock_circuit() override {}

            private:
                void add_internal_components();

                user_sound_component& m_component;
                abstract_gui_component* m_input;
                abstract_gui_component* m_output;
        };

    }   /*  Gui */

} /* Gammou */

#endif  /*  USER_COMPONENT_CIRCUIT */
