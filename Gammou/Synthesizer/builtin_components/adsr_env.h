#ifndef GAMMOU_ADSR_H_
#define GAMMOU_ADSR_H_

#include "../../debug.h"
#include "../plugin_management/abstract_plugin_factory.h"

#include "builtin_components.h"

namespace Gammou {

	namespace Sound {

		class adsr_env : public sound_component {

		public:
			adsr_env(const unsigned int channel_count);
			~adsr_env() {}

			void initialize_process() override;
			void process(const double input[]) override;

		private:
			multi_channel_variable<double> m_gate_fact;
			multi_channel_variable<unsigned int> m_tau_index;
		};
		
		BUILTIN_COMPONENT_INFO(
			adsr_env,
			adsr_env_component_id,
			"AdsrEnv",
			ComponentCategory::Envelope)

	} /* Sound */
} /* Gammou */


#endif
