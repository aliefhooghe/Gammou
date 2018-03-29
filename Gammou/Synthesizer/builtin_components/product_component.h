#ifndef PRODUCT_COMPONENT_H_
#define PRODUCT_COMPONENT_H_

#include "../plugin_management/abstract_plugin_factory.h"
#include "builtin_components_ids.h"

namespace Gammou {

	namespace Sound {

		namespace Builtin {

			class product_component : public sound_component {

			public:
				product_component(const unsigned int channel_count);
				~product_component() {}

				void process(const double input[]) override;
			};

			class product_factory : public default_plugin_factory<product_component> {

			public:
				product_factory()
					: default_plugin_factory<product_component>("Product", "Calculus", product_component_id) {}
				~product_factory() {}
			};

		} /* Builtin */

	} /* Sound */

} /* Gammou */

#endif