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

			class product_factory : public plugin_factory {

			public:
				product_factory();
				~product_factory() {}

			protected:
				abstract_sound_component *create_sound_component(data_source& source, const unsigned int channel_count) override;
				abstract_sound_component *create_sound_component(const abstract_form_answer& answer_form, const unsigned int channel_count) override;
			};

		} /* Builtin */

	} /* Sound */

} /* Gammou */

#endif