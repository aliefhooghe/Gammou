
#include "channels_manager.h"
#include "sound_component.h"


namespace gammou{

channels_manager::channels_manager(const unsigned int channel_count)
	: link_monitor(this), m_channel_count(channel_count), m_current_working_channel(0)
{

}

channels_manager::~channels_manager()
{

}

void channels_manager::register_sound_component(abstract_sound_component *component)
{
	plug_link(&(component->m_channels_manager_link));
}

unsigned int channels_manager::get_channel_count() const
{
	return m_channel_count;
}

unsigned int channels_manager::get_current_working_channel() const
{
	return m_current_working_channel;
}

void channels_manager::set_current_working_channel(const unsigned int channel)
{
	if( channel != m_current_working_channel ){
		m_current_working_channel = channel;
		send_tick();	//	TO make all sound_module_manager_link know the change
	}
}


/*
 *
 */


channels_manager_link::channels_manager_link()
	: process::link<channels_manager>(),
	  m_current_working_channel(0)
{
}

channels_manager_link::~channels_manager_link()
{
}

unsigned int channels_manager_link::get_current_working_channel() const
{
	return m_current_working_channel;
}

void channels_manager_link::on_tick()
{
	channels_manager *manager = get_link_source();

	if( manager != nullptr ){
		m_current_working_channel = manager->get_current_working_channel();
	}
}

}



