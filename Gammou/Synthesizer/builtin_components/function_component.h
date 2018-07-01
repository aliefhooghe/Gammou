#ifndef GAMMOU_FUNCTION_COMPONENT_H_
#define GAMMOU_FUNCTION_COMPONENT_H_

#include "../../debug.h"
#include "../plugin_management/abstract_plugin_factory.h"
#include "sound_processing_helper.h"

#include "builtin_components_ids.h"

namespace Gammou {

	namespace Sound {

		namespace Builtin {

			template<double func(double)>
			class function_component : public sound_component {

			public:
				function_component(
					const unsigned int channel_count)
					: sound_component("****", 1, 1, channel_count)
				{}
				~function_component() {}

				void process(const double input[]) override
				{
					m_output[0] = func(input[0]);
				}
			};

			template<double func(double)>
			class function_factory : 
				public default_plugin_factory<function_component<func> > 
			{
				public:
					function_factory(const std::string& name, const unsigned int id)
						:	default_plugin_factory<function_component<func> >
								(name, "Calculus", id)
					{}	
					~function_factory() {}
			};

		} /* Builtin */

	} /* Sound */

} /* Gammou */


#endif