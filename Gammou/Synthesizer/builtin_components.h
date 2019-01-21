#ifndef GAMMOU_BUILT_IN_COMPONENT_H_
#define GAMMOU_BUILT_IN_COMPONENT_H_

/*
			BUILTIN COMPONENTS
*/

#define BUILTIN_COMPONENT_INFO(type, id, n, c)			 \
	template<>											 \
	struct builtin_component_info<type> {				 \
	static constexpr unsigned int factory_id = id;		 \
	static constexpr auto category = c;					 \
	static constexpr auto name = n;};

#define MAKE_BUILTIN_FACTORY(type)							\
	std::make_unique<Sound::default_plugin_factory<type> >(		\
		Sound::builtin_component_info<type>::name,				\
		Sound::builtin_component_info<type>::category,			\
		Sound::builtin_component_info<type>::factory_id)

namespace Gammou {
	namespace Sound {
		template<class T> struct builtin_component_info;
	}
}

#include "builtin_components/builtin_components_ids.h"

#include "builtin_components/sin_component.h"
#include "builtin_components/sum_component.h"
#include "builtin_components/product_component.h"
#include "builtin_components/filter_order2.h"
#include "builtin_components/adsr_env.h"
#include "builtin_components/saw.h"

#include "builtin_components/function_component.h"


#endif
