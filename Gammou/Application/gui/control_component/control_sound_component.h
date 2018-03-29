#ifndef GAMMOU_CONTROL_COMPONENT_H_
#define GAMMOU_CONTROL_COMPONENT_H_

#include "sound_component/sound_component.h"

namespace Gammou {

	namespace Gui {

		class control_sound_component : public Sound::sound_component {

		public:
			control_sound_component(
				const std::string& name,
				const unsigned int input_count,
				const unsigned int output_count,
				const unsigned int channel_count, 
				const double initial_normalized_value,
				const double amplitude,
				const double shape);

			~control_sound_component() {}

			unsigned int save_state(Sound::data_sink& data) override;
			void set_normalized_value(const double normalized_value);
			double get_normalized_value() const;

		protected: 
			double m_param_value;

		private:
			double m_normalized_value;
			const double m_amplitude;
			const double m_shape;
		};

	}


} /* Gammou */

#endif