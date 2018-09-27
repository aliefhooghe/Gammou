
#include "slider_gui_component.h"

#define SLIDER_TAU 0.1
#define SLIDER_AMPLITUDE 1.0
#define SLIDER_SHAPE 1.0
#define SLIDER_INITIAL_NORMALIZED_VALUE 0.2

namespace Gammou {

    namespace Gui {

        slider_gui_component::slider_gui_component(
            control_sound_component *control, 
			const unsigned int x, 
            const unsigned int y,
            const unsigned int width)
        :   gui_sound_component(
                std::unique_ptr<Sound::abstract_sound_component>(control), x, y),
            m_control(control)
        {
            set_autosize(false);
            resize(width, 50);

            const unsigned int y_offset = 22;
            const unsigned int x_offset = 20;

            auto slider_control = 
                std::make_unique<View::slider>(
                    [&](View::slider& self)
                    {
                        m_control->set_normalized_value(
                            self.get_normalized_value());
                    },
                    x_offset, y_offset,
                    get_width() - 2 * x_offset,
					GuiProperties::slider_on_color,
					GuiProperties::slider_off_color);
            
            slider_control->set_normalized_value(control->get_normalized_value());
            add_widget(std::move(slider_control));
		}



        // value_slider_gui_component_factory implementation

		value_slider_gui_component_factory::value_slider_gui_component_factory()
			: abstract_gui_component_factory("Slider", "Control", control_ids::slider_value_id)
		{
		}

		std::unique_ptr<gui_sound_component> value_slider_gui_component_factory::create_gui_component(
			const int x, const int y, Sound::data_input_stream & source, const unsigned int channel_count)
		{
			// Load normalized value
			double normalized_value;
			source.read(&normalized_value, sizeof(double));

			control_sound_component *sound_component = 
				new value_sound_component(
                    "Slider", channel_count, normalized_value,
                     SLIDER_AMPLITUDE, SLIDER_SHAPE, SLIDER_TAU);
			stamp_sound_component(sound_component);
			
			return std::make_unique<slider_gui_component>(sound_component, x, y);
		}

		std::unique_ptr<gui_sound_component> value_slider_gui_component_factory::create_gui_component(
			const int x, const int y, const Sound::answer_form & answer_form, const unsigned int channel_count)
		{
			control_sound_component *sound_component =
				new value_sound_component("Slider", channel_count,
					SLIDER_INITIAL_NORMALIZED_VALUE, 
					SLIDER_AMPLITUDE, SLIDER_SHAPE, SLIDER_TAU);
			stamp_sound_component(sound_component);

			return std::make_unique<slider_gui_component>(sound_component, x, y);
		}

		// gain_slider_gui_component_factory Implementation

		gain_slider_gui_component_factory::gain_slider_gui_component_factory()
			: abstract_gui_component_factory("Gain Slider", "Control", control_ids::slider_gain_id)
		{
		}

		std::unique_ptr<gui_sound_component> gain_slider_gui_component_factory::create_gui_component(
			const int x, const int y, Sound::data_input_stream & source, const unsigned int channel_count)
		{
			// Load normalized value
			double normalized_value;
			source.read(&normalized_value, sizeof(double));

			control_sound_component *sound_component =
				new gain_sound_component(
                    "Gain Slider", channel_count, normalized_value, 
                    SLIDER_AMPLITUDE, SLIDER_SHAPE, SLIDER_TAU);
			stamp_sound_component(sound_component);

			return std::make_unique<slider_gui_component>(sound_component, x, y);
		}

		std::unique_ptr<gui_sound_component> gain_slider_gui_component_factory::create_gui_component(
			const int x, const int y, const Sound::answer_form & answer_form, const unsigned int channel_count)
		{
			control_sound_component *sound_component =
				new gain_sound_component("Gain Slider", channel_count,
					SLIDER_INITIAL_NORMALIZED_VALUE,
					SLIDER_AMPLITUDE, SLIDER_SHAPE, SLIDER_TAU);
			stamp_sound_component(sound_component);
			
			return std::make_unique<slider_gui_component>(sound_component, x, y);
		}

    }   /* View */

}   /* Gammou */