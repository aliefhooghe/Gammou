

#include "knob_gui_component.h"

#define KNOB_TAU 0.1
#define KNOB_AMPLITUDE 5000.0
#define KNOB_SHAPE 22000.0
#define KNOB_INITIAL_NORMALIZED_VALUE 0.2

namespace Gammou {

	namespace Gui {

		knob_gui_component::knob_gui_component(Sound::midi_driver& driver, control_sound_component * control, const unsigned int x, const unsigned int y)
			:
			gui_sound_component(
				std::unique_ptr<Sound::abstract_sound_component>(control), x, y),
			m_control(control),
			m_midi_driver(driver)
		{
			set_autosize(false);
			resize(get_width(), get_width());

			const auto knob_size = 50;
			const int pos = (get_width() - knob_size) / 2;

			auto knob_control = 
				std::make_unique<View::knob>(
					[&](View::knob *self)
					{
						m_control->set_normalized_value(
                            static_cast<double>(self->get_normalized_value()));
					},
					pos, pos, knob_size);
			GuiProperties::apply(*knob_control);

			const auto button_size = 15;
			auto midi_learn_button =
				std::make_unique<View::push_button>(
					[this, knob = knob_control.get()](View::push_button*)
					{
						m_midi_driver.learn_CC(
							[this, knob](double val)
							{
								m_control->set_normalized_value(val);
								knob->set_normalized_value(val);
								redraw();
							},
							reinterpret_cast<intptr_t>(this)
						);

					},
					"L",
					get_width() - (pos + knob_size + button_size), pos + knob_size, // x, y
					button_size, button_size, // w , h
					10,	// font size
					GuiProperties::main_gui_list_box_hovered_item_color,
					GuiProperties::main_gui_list_box_selected_item_color,
					GuiProperties::link_color
				);

            knob_control->set_normalized_value(
                static_cast<float>(control->get_normalized_value()));
			add_widget(std::move(knob_control));
			add_widget(std::move(midi_learn_button));
		}

		knob_gui_component::~knob_gui_component()
		{
			m_midi_driver.unmap_CC(reinterpret_cast<intptr_t>(this));
		}

		// value_knob_gui_component_factory implementation

		value_knob_gui_component_factory::value_knob_gui_component_factory(Sound::midi_driver& driver)
            : abstract_gui_component_factory("Knob", ControlCategory, control_ids::knob_value_id),
				m_midi_driver{driver}
		{
		}

		std::unique_ptr<gui_sound_component> value_knob_gui_component_factory::create_gui_component(
			const int x, const int y, Sound::data_input_stream & source, const unsigned int channel_count)
		{
			// Load normalized value
			double normalized_value;
			source.read(&normalized_value, sizeof(double));

			control_sound_component *sound_component = 
				new value_sound_component("Knob", channel_count, normalized_value, KNOB_AMPLITUDE, KNOB_SHAPE, KNOB_TAU);
			stamp_sound_component(sound_component);

			return std::make_unique<knob_gui_component>(m_midi_driver, sound_component, x, y);
		}

		std::unique_ptr<gui_sound_component> value_knob_gui_component_factory::create_gui_component(
            const int x, const int y, const Sound::answer_form&, const unsigned int channel_count)
		{
			// No answer needed, default knob

			control_sound_component *sound_component =
				new value_sound_component("Knob", channel_count,
					KNOB_INITIAL_NORMALIZED_VALUE, 
					KNOB_AMPLITUDE, KNOB_SHAPE, KNOB_TAU);
			stamp_sound_component(sound_component);

			return std::make_unique<knob_gui_component>(m_midi_driver, sound_component, x, y);
		}

		// gain_knob_gui_component_factory Implementation

		gain_knob_gui_component_factory::gain_knob_gui_component_factory(Sound::midi_driver& driver)
            : abstract_gui_component_factory("Gain Knob", ControlCategory, control_ids::knob_gain_id),
				m_midi_driver{driver}
		{
		}

		std::unique_ptr<gui_sound_component> gain_knob_gui_component_factory::create_gui_component(
			const int x, const int y, Sound::data_input_stream & source, const unsigned int channel_count)
		{
			// Load normalized value
			double normalized_value;
			source.read(&normalized_value, sizeof(double));

			control_sound_component *sound_component =
				new gain_sound_component("Gain Knob", channel_count, normalized_value, KNOB_AMPLITUDE, KNOB_SHAPE, KNOB_TAU);
			stamp_sound_component(sound_component);

			return std::make_unique<knob_gui_component>(m_midi_driver, sound_component, x, y);
		}

		std::unique_ptr<gui_sound_component> gain_knob_gui_component_factory::create_gui_component(
            const int x, const int y, const Sound::answer_form&, const unsigned int channel_count)
		{
			// No answer needed, default knob

			control_sound_component *sound_component =
				new gain_sound_component("Gain Knob", channel_count,
					KNOB_INITIAL_NORMALIZED_VALUE,
					KNOB_AMPLITUDE, KNOB_SHAPE, KNOB_TAU);
			stamp_sound_component(sound_component);

			return std::make_unique<knob_gui_component>(m_midi_driver, sound_component, x, y);
		}
	}

}
