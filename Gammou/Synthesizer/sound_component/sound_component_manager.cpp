
#include "sound_component_manager.h"
#include "sound_component.h"


namespace Gammou {

	namespace Sound {

		sound_component_manager::sound_component_manager(const unsigned int channel_count)
			: Process::subject<sound_component_manager, sound_component_notification_tag>(this), m_channel_count(channel_count),
			m_current_working_channel(0),
			m_current_sample_rate(DEFAULT_SAMPLE_RATE)
		{

		}

		sound_component_manager::~sound_component_manager()
		{

		}

		unsigned int sound_component_manager::get_channel_count() const
		{
			return m_channel_count;
		}

		unsigned int sound_component_manager::get_current_working_channel() const
		{
			return m_current_working_channel;
		}

		double sound_component_manager::get_current_sample_rate() const
		{
			return m_current_sample_rate;
		}

		void sound_component_manager::set_current_working_channel(const unsigned int channel)
		{
			if( channel != m_current_working_channel ){
				m_current_working_channel = channel;
				notify_observers(sound_component_notification_tag::CHANNEL_CHANGE_NOTIFY);
			}
		}

		void sound_component_manager::set_current_samplerate(const double sample_rate)
		{
			m_current_sample_rate = sample_rate;
			notify_observers(sound_component_notification_tag::SAMPLE_RATE_NOTIFY);
		}


	} /* oOund */


} /* Gammou */



