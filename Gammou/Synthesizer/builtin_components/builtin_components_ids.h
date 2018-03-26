#ifndef GAMMOU_BUILTIN_COMPONENTS_IDS_H_
#define GAMMOU_BUILTIN_COMPONENTS_IDS_H_


namespace Gammou {

	namespace Sound {

		namespace Builtin {

			enum builtin_ids : unsigned int {
				sin_component_id		= 1000u,
				adsr_env_component_id	= 1001u,
				sum_component_id		= 1002u,
				lp2_component_id		= 1003u,
				product_component_id	= 1004u,
				saw_component_id		= 1005u,
			//	noise_component_id = 1006u
			};

		}
	} /* Sound */

} /* Gammou */

#endif