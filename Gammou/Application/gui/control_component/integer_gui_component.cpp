#include "integer_gui_component.h"

namespace Gammou {

	namespace Gui {

		integer_gui_component::integer_gui_component(
			integer_sound_component * integer, 
			const unsigned int x, 
			const unsigned int y)
		: gui_sound_component(
				std::unique_ptr<Sound::abstract_sound_component>
				(integer), x, y),
			m_integer(integer)
		{
			const unsigned int size = get_width();
			const unsigned int unit = (size / 10);
			
			set_autosize(false);
			resize(size, size);

			auto label =
				std::make_unique<View::label>(
				"",
				2 * unit, 2 * unit,
				6 * unit, 2 * unit,
				GuiProperties::component_font_color
			);
			m_label = label.get();
			update_label();

			auto dec =
				std::make_unique<View::push_button>(
				[this](View::push_button *self) 
				{ 
					m_integer->shift_value(-1); 
					update_label();
				},
				"-",
				1.5 * unit, 5 * unit, // x, y
				3 * unit, 3 * unit, // w , h
				10,	// font size
				GuiProperties::main_gui_list_box_hovered_item_color,
				GuiProperties::main_gui_list_box_selected_item_color,
				GuiProperties::component_font_color
			);
			m_dec = dec.get();

			auto inc =
				std::make_unique<View::push_button>(
				[this](View::push_button *self) 
				{ 
					m_integer->shift_value(1); 
					update_label();
				},
				"+",
				5.5 * unit, 5 * unit, // x, y
				3 * unit, 3 * unit, // w , h
				10,	// font size
				GuiProperties::main_gui_list_box_hovered_item_color,
				GuiProperties::main_gui_list_box_selected_item_color,
				GuiProperties::component_font_color
			);
			m_inc = inc.get();

			add_widget(std::move(label));
			add_widget(std::move(dec));
			add_widget(std::move(inc));
		}

		bool integer_gui_component::on_mouse_dbl_click(const int x, const int y)
		{
			View::widget *w = get_focused_widget();
			return ((w == m_dec) || (w == m_inc));
		}

		void integer_gui_component::update_label()
		{
			const auto value =
				m_integer->get_value();

			if (value >= 0)
				m_label->set_text(std::to_string(value));
			else {
				std::string text =
					"1/" + std::to_string(-value);
				m_label->set_text(text);
			}
		}


		/* Value Integer Factory */

		value_integer_gui_component_factory::value_integer_gui_component_factory()
            : abstract_gui_component_factory("Integer", ControlCategory, control_ids::integer_value_id)
		{
		}

		std::unique_ptr<gui_sound_component> value_integer_gui_component_factory::create_gui_component(
			const int x, const int y, 
			Sound::data_input_stream & source, const unsigned int channel_count)
		{
			int value;
			source.read(&value, sizeof(int));
			
			integer_sound_component *sound_component =
                new value_integer_sound_component("Integer", channel_count, value);

			stamp_sound_component(sound_component);

			return	std::make_unique<integer_gui_component>(sound_component, x, y);
		}

		std::unique_ptr<gui_sound_component> value_integer_gui_component_factory::create_gui_component(
			const int x, const int y, 
			const Sound::answer_form & answer_form, const unsigned int channel_count)
		{
			integer_sound_component *sound_component =
                new value_integer_sound_component("Integer", channel_count, 0);

			stamp_sound_component(sound_component);

			return	std::make_unique<integer_gui_component>(sound_component, x, y);
		}

		// Gain Integer implementation

		gain_integer_gui_component_factory::gain_integer_gui_component_factory()
            : abstract_gui_component_factory("Gain Integer", ControlCategory, control_ids::integer_gain_id)
		{
		}

		std::unique_ptr<gui_sound_component> gain_integer_gui_component_factory::create_gui_component(
			const int x, const int y,
			Sound::data_input_stream & source, 
			const unsigned int channel_count)
		{
			int value;
			source.read(&value, sizeof(int));

			integer_sound_component *sound_component =
				new gain_integer_sound_component("Gain Integer", channel_count, value);

			stamp_sound_component(sound_component);

			return	std::make_unique<integer_gui_component>(sound_component, x, y);
		}

		std::unique_ptr<gui_sound_component> gain_integer_gui_component_factory::create_gui_component(
			const int x, const int y,
			const Sound::answer_form & answer_form, const unsigned int channel_count)
		{
			integer_sound_component *sound_component =
				new gain_integer_sound_component("Gain Integer", channel_count, 0);

			stamp_sound_component(sound_component);

			return	std::make_unique<integer_gui_component>(sound_component, x, y);
		}

	} /* Gui */

} /* Gammou */
