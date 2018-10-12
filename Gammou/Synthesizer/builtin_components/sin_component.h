#ifndef SIN_COMPONENT_H_
#define SIN_COMPONENT_H_

#include <cmath>

#include "../plugin_management/abstract_plugin_factory.h"
#include "builtin_components.h"

namespace Gammou {

	namespace Sound {

		class sin_component : public sound_component {

		public:
			sin_component(const unsigned int channel_count);
			~sin_component();

			void process(const double input[]) override;
			void initialize_process() override;

		private:
			multi_channel_variable<double> m_frequence_integral;
		};

		BUILTIN_COMPONENT_INFO(
			sin_component,
			sin_component_id,
			"Sine",
			ComponentCategory::Oscillator)

	} /* Sound */

} /* Gammou */


#endif
