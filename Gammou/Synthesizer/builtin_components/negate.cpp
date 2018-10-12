
#include "negate.h"

namespace Gammou {

	namespace Sound {

		negate_component::negate_component(const unsigned int channel_count)
			: sound_component("negate", 1, 1, channel_count)
		{
		}

		void negate_component::process(const double input[])
		{
			m_output[0] = -input[0];
		}

	} /* Sound */

} /* Gammou */





