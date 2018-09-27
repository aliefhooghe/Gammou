#ifndef DEBUG_COMPONENT_H_
#define DEBUG_COMPONENT_H_

#include "../../debug.h"
#include "../plugin_management/abstract_plugin_factory.h"

#include "builtin_components_ids.h"

namespace Gammou {

	namespace Sound {

		namespace Builtin {

			class sum_component : public sound_component {

			public:
				sum_component(const unsigned int channel_count);
				~sum_component();

				void process(const double input[]) override;
				void on_input_connection(const unsigned int input_id) override;
				void on_input_deconnection(const unsigned int input_id) override;
			private:
				unsigned int free_input_count();

			};

			class sum_component_factory : public default_plugin_factory<sum_component> {

			public:
				sum_component_factory()
					: default_plugin_factory<sum_component>
                        ("Sum", ComponentCategory::Calculus, sum_component_id) {}
				~sum_component_factory() {}
			};

		} /* Builtin */

	} /* Sound */

} /* Gammou */

#endif
