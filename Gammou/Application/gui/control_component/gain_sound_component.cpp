
#include "gain_sound_component.h"


namespace Gammou {

	namespace Gui {
		
		gain_sound_component::gain_sound_component(
			const std::string & name, 
			const unsigned int channel_count, 
			const double initial_normalized_value, 
			const double amplitude, 
			const double shape,
			const double characteristic_time)
			: control_sound_component(
				name,
				1, 1,
				channel_count, 
				initial_normalized_value,
				amplitude, shape),
			m_gain(characteristic_time)
		{
			set_output_name("", 0);
			set_input_name("", 0);
		}


		void gain_sound_component::initialize_process()
		{
			m_gain.force_current_value(m_param_value);
		}

		void gain_sound_component::process(const double input[])
		{
			m_output[0] = m_gain * input[0];
			m_gain.tick(m_param_value, get_sample_duration());
		}

	} /* Gui */

} /* Gammou */