
#include "invert.h"

namespace Gammou {

	namespace Sound {

		invert_component::invert_component(const unsigned int channel_count)
			: sound_component("invert", 1, 1, channel_count)
		{
		}

		void invert_component::process(const double input[])
		{
			const double x = input[0];
			if (abs(x) <= FLT_EPSILON)
				m_output[0] = 1.0;
			else
				m_output[0] = 1.0 / input[0];
		}

	} /* Sound */

} /* Gammou */





