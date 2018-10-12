#ifndef DEBUG_COMPONENT_H_
#define DEBUG_COMPONENT_H_

#include "../../debug.h"
#include "../plugin_management/abstract_plugin_factory.h"

#include "builtin_components.h"

namespace Gammou {

	namespace Sound {

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

		BUILTIN_COMPONENT_INFO(
			sum_component,
			sum_component_id,
			"Sum",
			ComponentCategory::Calculus)

	} /* Sound */

} /* Gammou */

#endif
