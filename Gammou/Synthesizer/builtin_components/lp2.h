#ifndef GAMMOU_lp1_H_
#define GAMMOU_lp1_H_

#include "../../debug.h"
#include "../plugin_management/abstract_plugin_factory.h"
#include "sound_processing_helper.h"

#include "builtin_components.h"

namespace Gammou {

	namespace Sound {

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

		BUILTIN_COMPONENT_INFO(
			lp2,
			lp2_component_id,
			"Lp2",
			ComponentCategory::Filter)

	} /* Sound */

} /* Gammou */

#endif
