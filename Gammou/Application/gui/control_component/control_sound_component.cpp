

#include <cmath>
#include "control_sound_component.h"

namespace Gammou {

	namespace Gui {

		control_sound_component::control_sound_component(
			const std::string & name,
			const unsigned int input_count,
			const unsigned int output_count,
			const unsigned int channel_count,
			const double initial_normalized_value,
			const double amplitude, const double shape)
			: Sound::sound_component(name, input_count, output_count, channel_count),
				m_amplitude(amplitude),
				m_shape(shape)
		{
			set_normalized_value(initial_normalized_value);
		}

		unsigned int control_sound_component::save_state(Sound::data_output_stream & data)
		{
			return data.write(&m_normalized_value, sizeof(double));
		}

		void control_sound_component::set_normalized_value(const double normalized_value)
		{
			m_normalized_value = normalized_value;

			if (0.99 < m_shape && m_shape < 1.01)
				m_param_value = m_normalized_value * m_amplitude;
			else
				m_param_value = m_amplitude * (std::pow(m_shape, normalized_value) - 1.0) / (m_shape - 1.0);
		}

		double control_sound_component::get_normalized_value() const
		{
			return m_normalized_value;
		}


	} /* Gui */

} /* Gammou */
