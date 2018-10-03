#ifndef USER_COMPONENT_EDITOR_H_
#define USER_COMPONENT_EDITOR_H_

#include <functional>
#include <view.h>
#include "gui_properties.h"
#include "user_sound_component.h"

namespace Gammou {

    namespace Gui {

        class user_component_editor : public View::panel<> {

            public:
                user_component_editor(
                    const int x,
                    const int y,
                    const unsigned int width,
                    const unsigned int height);

                ~user_component_editor();

                void select_component_creation_factory_id(const unsigned int factory_id);
                void open_user_component(user_sound_component *component);
                void close_user_component();

                void set_component_open_action(std::function<void(void)> callback);

            private:
                View::widget *m_circuit_proxy{};
                user_sound_component *m_user_sound_component{};
                unsigned int m_creation_factory_id{Sound::NO_FACTORY};
                std::function<void(void)> m_on_component_open{};
        };
    }
}

#endif
