#ifndef SOUND_MODULE_MANAGER_H_
#define SOUND_MODULE_MANAGER_H_

#include "gammou_process.h"

namespace Gammou {

	namespace Sound {

		class abstract_sound_component;

		enum class sound_component_notification_tag {SAMPLE_RATE_NOTIFY};

		class sound_component_manager : private Process::subject<sound_component_manager, sound_component_notification_tag> {

		public:
            sound_component_manager(const unsigned int channel_count);
			~sound_component_manager();

			unsigned int get_channel_count() const;
			unsigned int get_current_working_channel() const;
			double get_current_sample_rate() const;

			void set_current_working_channel(const unsigned int channel);
			void set_current_samplerate(const double sample_rate);

			void register_sound_component(abstract_sound_component *component);
		private:
			const unsigned int m_channel_count;
			unsigned int m_current_working_channel;
			double m_current_sample_rate;
		};


	} /* Sound */

} /* Gammou */



#endif /* SOUND_MODULE_MANAGER_H_ */
