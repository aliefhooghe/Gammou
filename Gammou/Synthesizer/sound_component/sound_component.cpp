
#include "sound_component.h"



namespace Gammou {

	namespace Sound {


		abstract_sound_component::abstract_sound_component(
			const std::string & name, 
			const unsigned int input_count, 
			const unsigned int output_count)
			: Process::abstract_component<double>(name, input_count, output_count),
			m_sample_rate(DEFAULT_SAMPLE_RATE),
			m_sample_duration(DEFAULT_SAMPLE_DURATION),
			m_factory_id(NO_FACTORY)
		{
		}

		unsigned int abstract_sound_component::get_factory_id() const
		{
			return m_factory_id;
		}


		void abstract_sound_component::on_notify(const sound_component_notification_tag notification_tag)
		{
			sound_component_manager *manager = get_subject_resource();

			if (manager != nullptr) {

				switch (notification_tag) {
					
				case sound_component_notification_tag::SAMPLE_RATE_NOTIFY:
					m_sample_rate = manager->get_current_sample_rate();
					m_sample_duration = 1.0 / m_sample_rate;
					on_sample_rate_change(m_sample_rate);
					break;

				case sound_component_notification_tag::CHANNEL_CHANGE_NOTIFY:
					on_channel_change(manager->get_current_working_channel());
					break;

				default:
					break;

				}

			}
		}



		/*
		
		*/

		polyphonic_sound_component::polyphonic_sound_component(
				const std::string& name,
				const unsigned int input_count,
				const unsigned int output_count,
				const unsigned int channel_count)
				: abstract_sound_component(name, input_count, output_count),
				m_channels_count(channel_count),
				m_current_working_channel(0)
		{

		}


		unsigned int polyphonic_sound_component::get_channel_count() const
		{
			return m_channels_count;
		}


		void polyphonic_sound_component::on_channel_change(const unsigned int new_chanel)
		{
			m_current_working_channel = new_chanel;
		}


		/*
		*
		*/

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
