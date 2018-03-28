#ifndef GAMMOU_SOUD_COMPONENT_H_
#define GAMMOU_SOUD_COMPONENT_H_

#include "polyphonic_sound_component.h"
#include "multi_channel_data_container.h"

namespace Gammou {

	namespace Sound {


		class sound_component : public polyphonic_sound_component {

		public:

			sound_component(const std::string& name,
					const unsigned int input_count,
					const unsigned int output_count,
					const unsigned int channel_count);
			virtual ~sound_component() {}


			//	not virtual : if another behaviour is needed, inherit from polyphonic_sound_component
			double fetch_output(const unsigned int output_id) override;
		protected:
			multi_channel_array<double> m_output;

		};



	} /* Sound */


} /* Gammou */



#endif /* SOUD_MODULE_H_ */
