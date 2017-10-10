
#include "sound_component.h"


namespace gammou {


abstract_sound_component::abstract_sound_component(
		const std::string& name,
		const unsigned int input_count,
		const unsigned int output_count,
		const unsigned int channel_count,
		const bool is_multi_channel)
		: abstract_component<double>(name, input_count, output_count),
		  m_is_multi_channel(is_multi_channel),
		  m_channels_count(channel_count)
{

}

abstract_sound_component::~abstract_sound_component()
{

}

unsigned int abstract_sound_component::channel_count() const
{
	return m_is_multi_channel;
}

bool abstract_sound_component::is_multi_channel() const
{
	return m_is_multi_channel;
}

unsigned int abstract_sound_component::get_current_working_channel() const
{
	return m_channels_manager_link.get_current_working_channel();
}


/*
 *
 */

sound_component::sound_component(
						const std::string& name,
						const unsigned int input_count,
						const unsigned int output_count,
						const unsigned int channel_count,
						const bool is_multi_channel)
		:	abstract_sound_component(
				name,
				input_count,
				output_count,
				channel_count,
				is_multi_channel),
				m_output(this, output_count)
{
}

sound_component::~sound_component()
{
}

double sound_component::fetch_output(const unsigned int output_id)
{
	return m_output[output_id];
}


}  /* namespace gammou */
