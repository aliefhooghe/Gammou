#ifndef GAMMOU_POLYPHONIC_SOUND_COMPONENT_H_
#define GAMMOU_POLYPHONIC_SOUND_COMPONENT_H_

#include "abstract_sound_component.h"

namespace Gammou {

	namespace Sound {
		
		// class polyphonic_sound_component
		class polyphonic_sound_component : public abstract_sound_component {

			friend class multi_channel_data;
			
		public:
			polyphonic_sound_component(
								const std::string& name,
								const unsigned int input_count,
								const unsigned int output_count,
								const unsigned int channel_count);

			virtual ~polyphonic_sound_component() {}

			// Not virtual, should not be modified
			unsigned int get_channel_count() const override;
			void set_working_channel_ref(const unsigned int *channel_ref) override;
		protected:
			inline unsigned int get_current_working_channel() const 
			{
				if (m_current_working_channel == nullptr)
					return 0;
				else
					return *m_current_working_channel; 
			}

		private:
			const unsigned int m_channels_count;
			const unsigned int *m_current_working_channel;
		};

		////-///
		
	} /* Sound */
} /* Gammou */


#endif