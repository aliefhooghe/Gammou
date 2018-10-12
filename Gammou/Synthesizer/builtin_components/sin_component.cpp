
#include "sin_component.h"

namespace Gammou {

	namespace Sound {

		sin_component::sin_component(const unsigned int channel_count)
			: sound_component("Sine", 2, 1, channel_count),
			m_frequence_integral(this)
		{
			set_input_name("Pitch", 0);
			set_input_name("Phase", 1);
		}

		sin_component::~sin_component()
		{

		}

		void sin_component::process(const double input[])
		{
			m_output[0] = std::sin(6.28318530718 * m_frequence_integral + input[1]);
			m_frequence_integral += (input[0] * get_sample_duration());
		}

		void sin_component::initialize_process()
		{
			m_frequence_integral = 0.0;
		}

	} /* Sound */

} /* Gammou */