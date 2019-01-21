#ifndef GAMMOU_INVERT_H_
#define GAMMOU_INVERT_H_

#include "sound_component/sound_component.h"
#include "builtin_components.h"

namespace Gammou {

	namespace Sound {

		class invert_component : public sound_component {

		public:

			invert_component(const unsigned int channel_count);
			~invert_component() {}

			void process(const double input[]) override;
		};

		BUILTIN_COMPONENT_INFO(
			invert_component,
			invert_component_id,
			"Invert",
			ComponentCategory::Calculus)

	} /* Sound */

} /* Gamou */

#endif