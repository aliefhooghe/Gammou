#include "integer_gui_component.h"

namespace Gammou {

	namespace Gui {

		integer_gui_component::integer_gui_component(
			integer_sound_component * integer, 
			const unsigned int x, 
			const unsigned int y)
			: 
			gui_sound_component(integer, x, y),
			m_integer(integer)
		{
			const unsigned int size = get_width();
			const unsigned int unit = (size / 10);
			
			set_autosize(false);
			resize(size, size);

			auto label =
				std::make_unique<View::label>(
				std::to_string(integer->get_value()),
				2 * unit, 2 * unit,
				6 * unit, 2 * unit,
				View::cl_black
			);
			View::label *label_ptr = &(*label);

			auto dec =
				std::make_unique<View::push_button>(
				[this, label_ptr](View::push_button *self) 
				{ 
					m_integer->shift_value(-1); 
					label_ptr->set_text(std::to_string(m_integer->get_value()));
				},
				"-",
				1.5 * unit, 5 * unit, // x, y
				3 * unit, 3 * unit // w , h
			);
			m_dec = &(*dec);

			auto inc =
				std::make_unique<View::push_button>(
				[this, label_ptr](View::push_button *self) 
				{ 
					m_integer->shift_value(1); 
					label_ptr->set_text(std::to_string(m_integer->get_value()));
				},
				"+",
				5.5 * unit, 5 * unit, // x, y
				3 * unit, 3 * unit // w , h
			);
			m_inc = &(*inc);

			add_widget(std::move(label));
			add_widget(std::move(dec));
			add_widget(std::move(inc));
		}

		bool integer_gui_component::on_mouse_dbl_click(const int x, const int y)
		{
			View::widget *w = get_focused_widget();
			return ((w == m_dec) || (w == m_inc));
		}

		/* Value Integer Factory */

		value_integer_gui_component_factory::value_integer_gui_component_factory()
			: abstract_gui_component_factory("Value Integer", "", control_ids::integer_value_id)
		{
		}

		abstract_gui_component_factory::complete_component value_integer_gui_component_factory::create_complete_component(
			const int x, const int y, 
			Sound::data_source & source, const unsigned int channel_count)
		{
			int value;
			source.read(&value, sizeof(int));
			
			integer_sound_component *sound_component =
				new value_integer_sound_component("Value Integer", channel_count, value);

			stamp_sound_component(sound_component);

			return std::make_pair(
					std::make_unique<integer_gui_component>(sound_component, x, y),
					sound_component);
		}

		abstract_gui_component_factory::complete_component value_integer_gui_component_factory::create_complete_component(
			const int x, const int y, 
			const Sound::abstract_form_answer & answer_form, const unsigned int channel_count)
		{
			integer_sound_component *sound_component =
				new value_integer_sound_component("Value Integer", channel_count, 0);

			stamp_sound_component(sound_component);

			return std::make_pair(
					std::make_unique<integer_gui_component>(sound_component, x, y),
					sound_component);
		}

		// Gain Integer implementation

		gain_integer_gui_component_factory::gain_integer_gui_component_factory()
			: abstract_gui_component_factory("Gain Integer", "", control_ids::integer_gain_id)
		{
		}

		abstract_gui_component_factory::complete_component gain_integer_gui_component_factory::create_complete_component(
			const int x, const int y,
			Sound::data_source & source, const unsigned int channel_count)
		{
			int value;
			source.read(&value, sizeof(int));

			integer_sound_component *sound_component =
				new gain_integer_sound_component("Gain Integer", channel_count, value);

			stamp_sound_component(sound_component);

			return std::make_pair(
					std::make_unique<integer_gui_component>(sound_component, x, y),
					sound_component);
		}

		abstract_gui_component_factory::complete_component gain_integer_gui_component_factory::create_complete_component(
			const int x, const int y,
			const Sound::abstract_form_answer & answer_form, const unsigned int channel_count)
		{
			integer_sound_component *sound_component =
				new gain_integer_sound_component("Gain Integer", channel_count, 0);

			stamp_sound_component(sound_component);

			return std::make_pair(
					std::make_unique<integer_gui_component>(sound_component, x, y),
					sound_component);
		}

	} /* Gui */

} /* Gammou */