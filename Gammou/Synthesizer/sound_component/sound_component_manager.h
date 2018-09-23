#ifndef SOUND_MODULE_MANAGER_H_
#define SOUND_MODULE_MANAGER_H_

#include "gammou_process.h"

namespace Gammou {

	namespace Sound {

		class abstract_sound_component;

        enum class sound_component_notification_tag
        {
            SAMPLE_RATE_CHANGE_NOTIFY,
            CHANNEL_REF_CHANGE_NOTIFY
        };

        class abstract_sound_component_manager {

            public:
                abstract_sound_component_manager();
                virtual ~abstract_sound_component_manager();

                virtual const unsigned int *get_current_working_channel_ref() const = 0;
                virtual double get_current_sample_rate() const = 0;

                void register_sound_component(abstract_sound_component *component);
            protected:
                //  Sound Components Notification
                void notify_working_channel_ref_change();
                void notify_sample_rate_change();

            private:
                using subject = Process::subject<abstract_sound_component_manager, sound_component_notification_tag>;
                subject m_subject;
        };

        class sound_component_manager :
                public abstract_sound_component_manager {

		public:
            sound_component_manager(const unsigned int channel_count);
			~sound_component_manager();

			unsigned int get_channel_count() const;

            const unsigned int *get_current_working_channel_ref() const override;
            double get_current_sample_rate() const override;

			void set_current_working_channel(const unsigned int channel);
			void set_current_samplerate(const double sample_rate);

		private:
			const unsigned int m_channel_count;
			unsigned int m_current_working_channel;
			double m_current_sample_rate;
		};


	} /* Sound */

} /* Gammou */



#endif /* SOUND_MODULE_MANAGER_H_ */
