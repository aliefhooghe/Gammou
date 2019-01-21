#ifndef GAMMOU_FILTER_ORDER2_H_
#define GAMMOU_FILTER_ORDER2_H_


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

		private:
            double yd2(
            	const double in,
				const double omega0, const double Q,
            	const double y, const double yd);

			multi_channel_variable<double> m_previous_output_deriv;
		};

		BUILTIN_COMPONENT_INFO(
			lp2,
			lp2_component_id,
			"LP2",
			ComponentCategory::Filter)

		//-----------------------

		class hp2 : public sound_component {

			public:
				hp2(const unsigned int channel_count);
				~hp2() {}

				void initialize_process() override;
				void process(const double input[]) override;

			private:
				double yd2(
					const double xd2,
					const double omega0, const double Q,
					const double y, const double yd);

				multi_channel_variable<double> m_previous_output_deriv;
				multi_channel_array<double> m_previous_input;
		};

		BUILTIN_COMPONENT_INFO(
			hp2,
			hp2_component_id,
			"HP2",
			ComponentCategory::Filter)

		//-----------------------

		class bp2 : public sound_component {

		  public:
			bp2(const unsigned int channel_count);
			~bp2() {}

			void initialize_process() override;
			void process(const double input[]) override;

			private:
			  	double yd2(
					const double xd,
					const double omega0, const double Q,
					const double y, const double yd);

				multi_channel_variable<double> m_previous_output_deriv;
				multi_channel_variable<double> m_previous_input;
		};

		BUILTIN_COMPONENT_INFO(
			bp2,
			bp2_component_id,
			"BP2",
			ComponentCategory::Filter)

	} /* Sound */

} /* Gammou */

#endif
