

#include "knob_gui_component.h"

#define KNOB_TAU 0.1
#define KNOB_AMPLITUDE 5000.0
#define KNOB_SHAPE 22000.0
#define KNOB_INITIAL_NORMALIZED_VALUE 0.2

namespace Gammou {

	namespace Gui {

		knob_gui_component::knob_gui_component(control_sound_component * control, const unsigned int x, const unsigned int y)
			: gui_sound_component(std::unique_ptr<Sound::abstract_sound_component>(control), x, y),
			m_control(control)
		{
			set_autosize(false);
			resize(get_width(), get_width());

			const int pos = (get_width() - 50) / 2;

			auto knob_control = 
				std::make_unique<View::knob>(
					[&](View::knob *self)
					{
						m_control->set_normalized_value(self->get_normalized_value());
					},
					pos, pos,
					GuiProperties::knob_on_color,
					GuiProperties::knob_off_color);

			knob_control->set_normalized_value(control->get_normalized_value());
			add_widget(std::move(knob_control));
		}
		// value_knob_gui_component_factory implementation

		value_knob_gui_component_factory::value_knob_gui_component_factory()
			: abstract_gui_component_factory("Knob", "Control", control_ids::knob_value_id)
		{
		}

		std::unique_ptr<gui_sound_component> value_knob_gui_component_factory::create_complete_component(
			const int x, const int y, Sound::data_source & source, const unsigned int channel_count)
		{
			// Load normalized value
			double normalized_value;
			source.read(&normalized_value, sizeof(double));

			control_sound_component *sound_component = 
				new value_sound_component("Knob", channel_count, normalized_value, KNOB_AMPLITUDE, KNOB_SHAPE, KNOB_TAU);
			stamp_sound_component(sound_component);
			
			return std::make_unique<knob_gui_component>(sound_component, x, y);
		}

		std::unique_ptr<gui_sound_component> value_knob_gui_component_factory::create_complete_component(
			const int x, const int y, const Sound::answer_form & answer_form, const unsigned int channel_count)
		{
			// No answer needed, default knob

			control_sound_component *sound_component =
				new value_sound_component("Knob", channel_count,
					KNOB_INITIAL_NORMALIZED_VALUE, 
					KNOB_AMPLITUDE, KNOB_SHAPE, KNOB_TAU);
			stamp_sound_component(sound_component);

			return std::make_unique<knob_gui_component>(sound_component, x, y);
		}

		// gain_knob_gui_component_factory Implementation

		gain_knob_gui_component_factory::gain_knob_gui_component_factory()
			: abstract_gui_component_factory("Gain Knob", "Control", control_ids::knob_gain_id)
		{
		}

		std::unique_ptr<gui_sound_component> gain_knob_gui_component_factory::create_complete_component(
			const int x, const int y, Sound::data_source & source, const unsigned int channel_count)
		{
			// Load normalized value
			double normalized_value;
			source.read(&normalized_value, sizeof(double));

			control_sound_component *sound_component =
				new gain_sound_component("Gain Knob", channel_count, normalized_value, KNOB_AMPLITUDE, KNOB_SHAPE, KNOB_TAU);
			stamp_sound_component(sound_component);

			return std::make_unique<knob_gui_component>(sound_component, x, y);
		}

		std::unique_ptr<gui_sound_component> gain_knob_gui_component_factory::create_complete_component(
			const int x, const int y, const Sound::answer_form & answer_form, const unsigned int channel_count)
		{
			// No answer needed, default knob

			control_sound_component *sound_component =
				new gain_sound_component("Gain Knob", channel_count,
					KNOB_INITIAL_NORMALIZED_VALUE,
					KNOB_AMPLITUDE, KNOB_SHAPE, KNOB_TAU);
			stamp_sound_component(sound_component);
			
			return std::make_unique<knob_gui_component>(sound_component, x, y);
		}
	}

}