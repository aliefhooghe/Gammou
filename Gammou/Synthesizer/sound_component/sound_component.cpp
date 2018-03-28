
#include "sound_component.h"

namespace Gammou {

	namespace Sound {


		sound_component::sound_component(
								const std::string& name,
								const unsigned int input_count,
								const unsigned int output_count,
								const unsigned int channel_count)
				:	polyphonic_sound_component(
						name,
						input_count,
						output_count,
						channel_count),
						m_output(this, output_count)
		{
		}

		double sound_component::fetch_output(const unsigned int output_id)
		{
			return m_output[output_id];
		}

	} /* Sound */

}  /* namespace Gammou */
