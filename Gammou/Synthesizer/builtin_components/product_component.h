#ifndef PRODUCT_COMPONENT_H_
#define PRODUCT_COMPONENT_H_

#include "../plugin_management/abstract_plugin_factory.h"
#include "builtin_components.h"

namespace Gammou {

	namespace Sound {

		class product_component : public sound_component {

		public:
			product_component(const unsigned int channel_count);
			~product_component() {}

			void process(const double input[]) override;
		};

		BUILTIN_COMPONENT_INFO(
			product_component,
			product_component_id,
			"Product",
			ComponentCategory::Calculus)

	} /* Sound */

} /* Gammou */

#endif
