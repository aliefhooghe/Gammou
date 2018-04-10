
#include "integer_control_sound_component.h"

namespace Gammou{

	namespace Gui {

		integer_sound_component::integer_sound_component(
			const std::string & name, 
			const unsigned int input_count,
			const unsigned int output_count,
			const unsigned int channel_count, 
			const int initial_value)
			: 
				Sound::sound_component(
					name, 
					input_count, 
					output_count, 
					channel_count)
		{
			set_value(initial_value);
		}

		unsigned int integer_sound_component::save_state(Sound::data_sink & data)
		{
			return data.write(&m_value, sizeof(int));
		}

		void integer_sound_component::set_value(const int v)
		{
			m_param = (v >= 0) ?
				static_cast<double>(v) :
				1.0 / static_cast<double>(-v);
			m_value = v;
		}

		void integer_sound_component::shift_value(const int offset)
		{
			set_value(m_value + offset);
		}

		int integer_sound_component::get_value() const
		{
			return m_value;
		}

		/* Value Integer */

		value_integer_sound_component::value_integer_sound_component(
			const std::string & name, 
			const unsigned int channel_count, 
			const int initial_value)
			: integer_sound_component(name, 0, 1, channel_count, initial_value)
		{
			set_output_name("", 0);
		}

		void value_integer_sound_component::process(const double input[])
		{
			m_output[0] = m_param;
		}

		/* Gain Integer */

		gain_integer_sound_component::gain_integer_sound_component(
			const std::string & name, 
			const unsigned int channel_count, 
			const int initial_value)
			: integer_sound_component(name, 1, 1, channel_count, initial_value)
		{
			set_input_name("", 0);
			set_output_name("", 0);
		}

		void gain_integer_sound_component::process(const double input[])
		{
			m_output[0] = input[0] * m_param;
		}

	} /* Gui */

} /* Gammou */