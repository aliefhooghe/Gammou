
#include "sound_component_manager.h"
#include "sound_component.h"


namespace Gammou {

	namespace Sound {

        /*
         *      abstract sound component manager implementation
         */

        abstract_sound_component_manager::abstract_sound_component_manager()
        :   m_subject(this)
        {
        }

        abstract_sound_component_manager::~abstract_sound_component_manager()
        {
        }

        void abstract_sound_component_manager::register_sound_component(abstract_sound_component *component)
        {
            m_subject.register_observer(component);
            component->set_working_channel_ref(
                        get_current_working_channel_ref());
            component->set_sample_rate(
                        get_current_sample_rate());
        }

        void abstract_sound_component_manager::notify_working_channel_ref_change()
        {
            m_subject.notify_observers(
                        sound_component_notification_tag::CHANNEL_REF_CHANGE_NOTIFY);
        }

        void abstract_sound_component_manager::notify_sample_rate_change()
        {
            m_subject.notify_observers(
                        sound_component_notification_tag::SAMPLE_RATE_CHANGE_NOTIFY);
        }

        /*
         *      sound component manager implementation
         */

		sound_component_manager::sound_component_manager(const unsigned int channel_count)
        :   m_channel_count(channel_count),
            m_current_working_channel(0),
            m_current_sample_rate(DEFAULT_SAMPLE_RATE)
		{
            if (channel_count == 0)
                throw std::range_error("Cannot create a component manager with 0 channel");
		}

		sound_component_manager::~sound_component_manager()
		{

		}

		unsigned int sound_component_manager::get_channel_count() const
		{
			return m_channel_count;
		}

        const unsigned int *sound_component_manager::get_current_working_channel_ref() const
        {
            return &m_current_working_channel;
        }

		double sound_component_manager::get_current_sample_rate() const
		{
			return m_current_sample_rate;
		}

		void sound_component_manager::set_current_working_channel(const unsigned int channel)
		{
			m_current_working_channel = channel;
		}

		void sound_component_manager::set_current_samplerate(const double sample_rate)
		{
			m_current_sample_rate = sample_rate;
            notify_sample_rate_change();
		}

	} /* oOund */


} /* Gammou */



