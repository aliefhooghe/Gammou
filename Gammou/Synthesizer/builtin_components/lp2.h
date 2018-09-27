#ifndef GAMMOU_lp1_H_
#define GAMMOU_lp1_H_

#include "../../debug.h"
#include "../plugin_management/abstract_plugin_factory.h"
#include "sound_processing_helper.h"

#include "builtin_components_ids.h"

namespace Gammou {

	namespace Sound {

		namespace Builtin {

			class lp2 : public sound_component {

			public:
				lp2(const unsigned int channel_count);
				~lp2() {}

				void initialize_process() override;
				void process(const double input[]) override;

                inline double yd2(
                    const double in,
                    const double omega0,
                    const double Q,
                    const double y, const double yd)
				{
					return (omega0 * omega0)
						* (in - ((1.0 / (omega0 * Q)) * yd) - y);
				}

			private:
				multi_channel_variable<double> m_previous_output_deriv;
			};

			class lp2_factory : public default_plugin_factory<lp2> {

			public:
				lp2_factory()
                    : default_plugin_factory<lp2>("lp2", ComponentCategory::Filter, lp2_component_id) {}
				~lp2_factory() {}
			};


		} /* Builtin */

	} /* Sound */

} /* Gammou */








#endif
