
#include "multi_channel_data_container.h"
#include "sound_component.h"

namespace gammou {

multi_channel_data::multi_channel_data(abstract_sound_component *owner)
	: m_owner(owner)
{
	if( owner->m_is_multi_channel ){
		m_channels_count = owner->m_channels_count;
	}
	else{
		m_channels_count = 1;
	}
}

unsigned int multi_channel_data::get_channels_count()
{
	return m_channels_count;
}

unsigned int multi_channel_data::get_current_working_channel()
{
	return m_owner->m_channels_manager_link.get_current_working_channel();
}



} /* gammou */
