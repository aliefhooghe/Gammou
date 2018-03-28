
#include "multi_channel_data_container.h"
#include "sound_component.h"

namespace Gammou {

	namespace Sound {

		multi_channel_data::multi_channel_data(polyphonic_sound_component *owner)
			: m_owner(owner)
		{
			m_channels_count = owner->get_channel_count();
			//DEBUG_PRINT("New Multi channel data (channel_count = %d\n", m_channels_count);
		}

	} /* Sound */

} /* Gammou */
