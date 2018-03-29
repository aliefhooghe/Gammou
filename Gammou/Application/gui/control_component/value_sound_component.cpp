
#include "value_sound_component.h"


namespace Gammou {

	namespace Gui {

		value_sound_component::value_sound_component(
			const std::string & name,
			const unsigned int channel_count,
			const double initial_normalized_value,
			const double amplitude,
			const double shape,
			const double characteristic_time)
			: control_sound_component(
				name,
				0, 1,
				channel_count,
				initial_normalized_value,
				amplitude, shape),
			m_value(characteristic_time)
		{
			set_output_name("", 0);
		}

		void value_sound_component::initialize_process()
		{
			m_value.force_current_value(m_param_value);
		}

		void value_sound_component::process(const double input[])
		{
			m_output[0] = m_value;
			m_value.tick(m_param_value, get_sample_duration());
		}

	} /* Gui */

} /* Gammou */