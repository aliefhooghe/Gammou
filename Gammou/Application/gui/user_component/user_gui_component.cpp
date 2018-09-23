
#include "user_gui_component.h"


namespace Gammou {

	namespace Gui {

        user_gui_component::user_gui_component(
            user_sound_component *component,
            const int x, const int y)
        :   gui_sound_component(
                std::unique_ptr<Sound::abstract_sound_component>(component), x, y),
            m_component(component)
        {
            set_autosize(false);
            resize(get_width(), get_width());

            auto button =
                std::make_unique<View::push_button>(
                    [this, component](View::push_button *self)
                    {
                        View::dialog_display display(*get_display(), component->get_gui_circuit());
                        display.open("BOOOOOOOOOM");
                        display.wait();
                    },
                    "Edit",
                    10, 10);

            add_widget(std::move(button));
        }


        //  Factory

        user_gui_component_factory::user_gui_component_factory(gui_component_main_factory& factory)
        :   abstract_gui_component_factory("UserComponent", "", 623983265),
            m_factory(factory)
        {
        }


        std::unique_ptr<gui_sound_component> user_gui_component_factory::create_complete_component(
            const int x, const int y, Sound::data_input_stream& source, const unsigned int channel_count)
        {
            user_sound_component *component =
                new user_sound_component("Nom", 3, 3, channel_count, m_factory);

            //component->load_state(source);

            return std::make_unique<user_gui_component>(component, x, y);
        }

        std::unique_ptr<gui_sound_component> user_gui_component_factory::create_complete_component(
            const int x, const int y, const Sound::answer_form& answer_form, const unsigned int channel_count)
        {
            user_sound_component *component =
                new user_sound_component("Nom", 3, 3, channel_count, m_factory);

            //component->load_state(source);

            return std::make_unique<user_gui_component>(component, x, y);
        }




	}   /*  Gui */

} /* Gammou */
