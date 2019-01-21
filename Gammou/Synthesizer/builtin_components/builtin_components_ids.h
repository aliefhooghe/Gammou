#ifndef GAMMOU_BUILTIN_COMPONENTS_IDS_H_
#define GAMMOU_BUILTIN_COMPONENTS_IDS_H_

#include "component_category.h"



namespace Gammou {

	namespace Sound {

		enum builtin_ids : unsigned int {
			
			//	Function_component
			cos_component_id = 100,
			exp_component_id,
			log_component_id,
			cosh_component_id,
			sinh_component_id,
			sqrt_component_id,
			fabs_component_id,
			atan_component_id,
            tanh_component_id,

			//---
			sin_component_id = 1000u,
			adsr_env_component_id,
			sum_component_id,
			lp2_component_id,
			product_component_id,
			saw_component_id,				
			negate_component_id,
			invert_component_id
		};

	
	} /* Sound */
} /* Gammou */

#endif
