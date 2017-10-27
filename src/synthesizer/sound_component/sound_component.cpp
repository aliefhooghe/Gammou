
#include "sound_component.h"



namespace gammou {


abstract_sound_component::abstract_sound_component(
		const std::string& name,
		const unsigned int input_count,
		const unsigned int output_count,
		const unsigned int channel_count)
		: abstract_component<double>(name, input_count, output_count),
		  m_channels_count(channel_count),
		  m_current_working_channel(0),
		  m_sample_rate(DEFAULT_SAMPLE_RATE),
		  m_sample_duration(DEFAULT_SAMPLE_DURATION)
{

}

abstract_sound_component::~abstract_sound_component()
{

}

unsigned int abstract_sound_component::get_channel_count() const
{
	return m_channels_count;
}

unsigned int abstract_sound_component::get_current_working_channel() const
{
	return m_current_working_channel;
}

void abstract_sound_component::on_notify(const unsigned int notification_tag)
{
	sound_component_manager *manager = get_subject_resource();

	if( manager != nullptr ){

		switch (notification_tag) {

		case sound_component_manager::CHANNEL_CHANGE_NOTIFY:
			m_current_working_channel = manager->get_current_working_channel();
			break;

		case sound_component_manager::SAMPLE_RATE_NOTIFY:
			m_sample_rate = manager->get_current_sample_rate();
			m_sample_duration = 1.0 / m_sample_rate;
			on_sample_rate_change();
			break;

		default:
			break;
		}

	}
}

double abstract_sound_component::get_sample_duration() const
{
	return m_sample_duration;
}

double abstract_sound_component::get_sample_rate() const
{
	return m_sample_rate;
}


/*
 *
 */

sound_component::sound_component(
						const std::string& name,
						const unsigned int input_count,
						const unsigned int output_count,
						const unsigned int channel_count)
		:	abstract_sound_component(
				name,
				input_count,
				output_count,
				channel_count),
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
