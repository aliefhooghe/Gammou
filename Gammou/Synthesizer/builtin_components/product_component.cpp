#include "product_component.h"

namespace Gammou {

	namespace Sound {

		product_component::product_component(const unsigned int channel_count)
			: sound_component("Product", 2, 1, channel_count)
		{
		}

		void product_component::process(const double input[])
		{
			m_output[0] = input[0] * input[1];
		}

	} /* Sound */

} /* Gammou */