
#include "polyphonic_sound_component.h"

namespace Gammou {

	namespace Sound {
		
		/*
		
		*/

		polyphonic_sound_component::polyphonic_sound_component(
				const std::string& name,
				const unsigned int input_count,
				const unsigned int output_count,
				const unsigned int channel_count)
				: abstract_sound_component(name, input_count, output_count),
				m_channels_count(channel_count),
				m_current_working_channel(nullptr)
		{

		}

		unsigned int polyphonic_sound_component::get_channel_count() const
		{
			return m_channels_count;
		}

		void polyphonic_sound_component::set_working_channel_ref(const unsigned int *channel_ref)
		{
			m_current_working_channel = channel_ref;
		}
		
	} /* Sound */
} /* Gammou */