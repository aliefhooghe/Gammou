
#include "abstract_sound_component.h"

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

		void abstract_sound_component::set_sample_rate(const double sample_rate)
		{
			m_sample_rate = sample_rate;
			m_sample_duration = 1.0 / sample_rate;
			on_sample_rate_change(m_sample_rate);
		}


		void abstract_sound_component::on_notify(const sound_component_notification_tag notification_tag)
		{
			sound_component_manager *manager = get_subject_resource();

			if (manager != nullptr) {

				switch (notification_tag) {
					
				case sound_component_notification_tag::SAMPLE_RATE_NOTIFY:
					set_sample_rate(manager->get_current_sample_rate());
					DEBUG_PRINT("Component '%s' updating sample rate to %lf\n", get_name().c_str(), m_sample_rate);
					break;

				default:
					break;

				}

			}
		}

		
	} /* Sound */
} /* Gammou */