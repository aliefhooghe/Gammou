
#include "user_component_editor.h"

namespace Gammou {

    namespace Gui {

        user_component_editor::user_component_editor(
            const int x,
            const int y,
            const unsigned int height,
            const unsigned int width)
        :   View::panel<>(x, y, width, height, View::cl_chartreuse)
        {

        }

        user_component_editor::~user_component_editor()
        {
        }

        void user_component_editor::open_user_component(user_sound_component *component)
        {
            close_user_component();

            auto proxy =
                std::make_unique<View::proxy_widget>(component->get_gui_circuit());

            proxy->set_pos(0, GuiProperties::user_component_editor_toolbox_height);
            component->select_component_creation_factory_id(m_creation_factory_id);

            m_user_sound_component = component;
            m_circuit_proxy = proxy.get();

            add_widget(std::move(proxy));

            if (m_on_component_open)
                m_on_component_open();
        }

        void user_component_editor::close_user_component()
        {
            if (m_circuit_proxy) {
                remove_widget(m_circuit_proxy);
                m_circuit_proxy = nullptr;
                m_user_sound_component = nullptr;
                redraw();
            }
        }

        void user_component_editor::set_component_open_action(std::function<void(void)> callback)
        {
            m_on_component_open = callback;
        }

        void user_component_editor::select_component_creation_factory_id(const unsigned int factory_id)
        {
            m_creation_factory_id = factory_id;

            if (m_user_sound_component)
                m_user_sound_component->select_component_creation_factory_id(factory_id);
        }

    } /* Gui */

} /* Gammou */
