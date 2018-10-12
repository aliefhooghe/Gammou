#ifndef GAMMOU_NEGATE_H_
#define GAMMOU_NEGATE_H_

#include "sound_component/sound_component.h"
#include "builtin_components.h"

namespace Gammou {

	namespace Sound {

		class negate_component : public sound_component {

		public:

			negate_component(const unsigned int channel_count);
			~negate_component() {}

			void process(const double input[]) override;
		};

		BUILTIN_COMPONENT_INFO(
			negate_component,
			negate_component_id,
			"Negate",
			ComponentCategory::Calculus)

	} /* Sound */

} /* Gamou */

#endif