
#include "user_gui_component.h"


namespace Gammou {

	namespace Gui {

        user_gui_component::user_gui_component(
            user_component_editor& editor,
            user_sound_component *component,
            const int x, const int y)
        :   gui_sound_component(
                std::unique_ptr<Sound::abstract_sound_component>(component), x, y),
            m_component(component),
            m_editor(editor)
        {

            auto button =
                std::make_unique<View::push_button>(
                    [this](View::push_button *)
                    {
                        m_editor.open_user_component(m_component);
                    },
                    "Edit",
                    20, 20, 40, 25);

            add_widget(std::move(button));
        }


        //  Factory

        user_gui_component_factory::user_gui_component_factory(
                user_component_editor& editor,
                gui_component_main_factory& factory)
        :   abstract_gui_component_factory("UserComponent", "", 623983265),
            m_factory(factory),
            m_editor(editor)
        {
        }


        std::unique_ptr<gui_sound_component> user_gui_component_factory::create_gui_component(
            const int x, const int y, Sound::data_input_stream& source, const unsigned int channel_count)
        {
            //  Load input and output count
            unsigned int input_count, output_count;

            source.read(&input_count, sizeof(input_count));
            source.read(&output_count, sizeof(output_count));

            user_sound_component *component =
                new user_sound_component("Nom", input_count, output_count, channel_count, m_factory);
            stamp_sound_component(component);

            //  Load circuit state
            component->load_circuit_state(source);

            return std::make_unique<user_gui_component>(m_editor, component, x, y);
        }

        std::unique_ptr<gui_sound_component> user_gui_component_factory::create_gui_component(
            const int x, const int y, const Sound::answer_form& answer_form, const unsigned int channel_count)
        {
            //  TODO : ask user to get input and output count
            user_sound_component *component =
                new user_sound_component("Nom", 3, 3, channel_count, m_factory);
            stamp_sound_component(component);

            return std::make_unique<user_gui_component>(m_editor, component, x, y);
        }




	}   /*  Gui */

} /* Gammou */
