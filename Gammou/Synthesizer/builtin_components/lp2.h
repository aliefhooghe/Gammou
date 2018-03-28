#ifndef GAMMOU_lp1_H_
#define GAMMOU_lp1_H_

#include "../../debug.h"
#include "../plugin_management/abstract_plugin_factory.h"
#include "sound_processing_helper.h"

#include "builtin_components_ids.h"

namespace Gammou {

	namespace Sound {

		namespace Builtin {

			// A netoyer

			class lp2 : public sound_component {

			public:
				lp2(const unsigned int channel_count);
				~lp2() {}

				void initialize_process() override;
				void process(const double input[]) override;

				inline double yd2(const double in, const double omega0, const double Q, const double y, const double yd)
				{
					return (omega0 * omega0)
						* (in - ((1.0 / (omega0 * Q)) * yd) - y);
				}

			private:
				multi_channel_variable<double> m_previous_output_deriv;
			};

			class lp2_factory : public plugin_factory {

			public:
				lp2_factory()
					: plugin_factory("lp2", "lp2", lp2_component_id) {}
				~lp2_factory() {}

			protected:
				abstract_sound_component *create_sound_component(data_source& source, const unsigned int channel_count) override
				{
					return new lp2(channel_count);
				}
				abstract_sound_component *create_sound_component(const abstract_form_answer& answer_form, const unsigned int channel_count) override
				{
					return new lp2(channel_count);
				}
			};


		} /* Builtin */

	} /* Sound */

} /* Gammou */








#endif