#ifndef GAMMOU_INTEGER_CONTROLSOUND_COMPONENT_H_
#define GAMMOU_INTEGER_CONTROLSOUND_COMPONENT_H_

#include "sound_component\sound_component.h"

namespace Gammou {

	namespace Gui {

		class integer_sound_component : public Sound::sound_component {

		public:
			integer_sound_component(
				const std::string& name,
				const unsigned int input_count,
				const unsigned int output_count,
				const unsigned int channel_count,
				const int initial_value
			);

			virtual ~integer_sound_component() {}

			unsigned int save_state(Sound::data_sink& data) override;

			void set_value(const int v);
			void shift_value(const int offset);
			int get_value() const;


		protected:
			double m_param;
		private:
			int m_value;
		};

		class value_integer_sound_component : public integer_sound_component {

		public:
			value_integer_sound_component(
				const std::string& name,
				const unsigned int channel_count,
				const int initial_value
			);

			~value_integer_sound_component() {}

			void process(const double input[]) override;
		};

		class gain_integer_sound_component : public integer_sound_component {

		public:
			gain_integer_sound_component(
				const std::string& name,
				const unsigned int channel_count,
				const int initial_value
			);

			~gain_integer_sound_component() {}

			void process(const double input[]) override;
		};



	} /* Gui */
} /* Gammou */




#endif