#ifndef GAMMOU_VALUE_SOUND_COMPONENT_H_
#define GAMMOU_VALUE_SOUND_COMPONENT_H_

#include "sound_processing_helper.h"
#include "control_sound_component.h"

namespace Gammou {

	namespace Gui {

		class value_sound_component : public control_sound_component {

		public:
			value_sound_component(
				const std::string& name,
				const unsigned int channel_count,
				const double initial_normalized_value,
				const double amplitude,
				const double shape,
				const double characteristic_time);

			~value_sound_component() {}

			void initialize_process() override;
			void process(const double input[]) override;

		private:
			Sound::value_smoother m_value;
		};

	} /* Gui */

} /* Gammou */


#endif