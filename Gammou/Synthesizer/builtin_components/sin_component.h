#ifndef SIN_COMPONENT_H_
#define SIN_COMPONENT_H_

#include <cmath>

#include "../plugin_management/abstract_plugin_factory.h"
#include "builtin_components_ids.h"

namespace Gammou {

	namespace Sound {

		namespace Builtin {

			class sin_component : public sound_component {

			public:
				sin_component(const unsigned int channel_count);
				~sin_component();

				void process(const double input[]) override;
				void initialize_process() override;

			private:
				multi_channel_variable<double> m_frequence_integral;
			};


			class sin_factory : public default_plugin_factory<sin_component> {

			public:
				sin_factory()
					: default_plugin_factory<sin_component>("Sine", "Oscillator", sin_component_id) {}
				~sin_factory() {}
			};

		} /* Built in */

	} /* Sound */

} /* Gammou */


#endif