#include "knob_component.h"

namespace Gammou {

	namespace Gui {

		knob_sound_component::knob_sound_component(const unsigned int channel_count,
			const double initial_normalized_value)
			: Sound::sound_component("Knob", 0, 1, channel_count),
				m_prev_out(this)
		{
			set_output_name("", 0);
			set_normalized_value(initial_normalized_value);
		}

		void knob_sound_component::initialize_process()
		{
			m_prev_out = m_output_value;
		}

		void knob_sound_component::process(const double input[])
		{
			const double tau = 0.1;
			const double fact = tau / get_sample_duration();
			const double out = (m_output_value + fact * m_prev_out) / (1.0 + fact);
			m_output[0] =  out;
			m_prev_out = out;
		}

		unsigned int knob_sound_component::save_state(Sound::data_sink & data)
		{
			return data.write(&m_normalized_value, sizeof(double));
		}

		void knob_sound_component::set_normalized_value(const double normalized_value)
		{
			m_normalized_value = normalized_value;
			m_output_value = 
				5000.0f * (expf(10.0f * normalized_value) - 1.0) / (expf(10.0f) - 1.0);
			
			const double v = m_output_value;
		}

		double knob_sound_component::get_normalized_value() const
		{
			return m_normalized_value;
		}


		// Gui component 

		knob_gui_component::knob_gui_component(knob_sound_component * knob, const unsigned int x, const unsigned int y)
			: gui_sound_component(knob, x, y),
				m_knob_sound_component(knob)
		{
			set_autosize(false);
			resize(get_width(), get_width());

			const int pos = (get_width() - 50) / 2;
			
			View::knob *knob_control = new View::knob(
				[&](View::knob *self)
				{
					m_knob_sound_component->set_normalized_value(self->get_normalized_value());
				},
				pos, pos,
				GuiProperties::knob_on_color,
				GuiProperties::knob_off_color);

			knob_control->set_normalized_value(knob->get_normalized_value());
			add_widget(knob_control);
		}

		//---

		knob_complete_component_factory::knob_complete_component_factory()
			: complete_component_factory("Knob", "Knob", 500)
		{
		}

		knob_complete_component_factory::complete_component 
			knob_complete_component_factory::create_complete_component(const int x, const int y, Sound::data_source & source, const unsigned int channel_count)
		{
			// Load normalized value;
			double normalized_value;
			source.read(&normalized_value, sizeof(double));

			knob_sound_component *sound_component = new knob_sound_component(channel_count, normalized_value);
			stamp_sound_component(sound_component);
			knob_gui_component *gui_component = new knob_gui_component(sound_component, x, y);
			return std::make_pair(gui_component, sound_component);
		}

		knob_complete_component_factory::complete_component 
			knob_complete_component_factory::create_complete_component(const int x, const int y, const Sound::abstract_form_answer & answer_form, const unsigned int channel_count)
		{
			// No answer needed, default knob
			knob_sound_component *sound_component = new knob_sound_component(channel_count);
			stamp_sound_component(sound_component);
			knob_gui_component *gui_component = new knob_gui_component(sound_component, x, y);
			return std::make_pair(gui_component, sound_component);
		}

	} /* Gui */

} /* Gammou */


