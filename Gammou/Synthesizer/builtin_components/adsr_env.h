#ifndef GAMMOU_ADSR_H_
#define GAMMOU_ADSR_H_

#include "../../debug.h"
#include "../plugin_management/abstract_plugin_factory.h"
#include "builtin_components_ids.h"

namespace Gammou {

	namespace Sound {

		namespace Builtin {

			// A netoyer

			class adsr_env : public sound_component {

			public:
				adsr_env(const unsigned int channel_count);
				~adsr_env() {}

				void initialize_process() override;
				void process(const double input[]) override;

			private:
				multi_channel_variable<double> m_gate_fact;
				multi_channel_variable<double> m_prev_output;
				multi_channel_variable<unsigned int> m_tau_index;
			};

			class adsr_env_factory : public plugin_factory {

			public:
				adsr_env_factory()
					: plugin_factory("AdsrEnv", "AdsrEnv", adsr_env_component_id) {}
				~adsr_env_factory() {}

			protected:
				abstract_sound_component *create_sound_component(data_source& source, const unsigned int channel_count) override
				{
					return new adsr_env(channel_count);
				}
				abstract_sound_component *create_sound_component(const abstract_form_answer& answer_form, const unsigned int channel_count) override
				{
					return new adsr_env(channel_count);
				}
			};


		} /* Builtin */
	} /* Sound */
} /* Gammou */


#endif