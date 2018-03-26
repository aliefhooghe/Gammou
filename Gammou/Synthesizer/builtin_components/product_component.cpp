#include "product_component.h"

namespace Gammou {

	namespace Sound {

		namespace Builtin {

			product_component::product_component(const unsigned int channel_count)
				: sound_component("Product", 2, 1, channel_count)
			{
			}

			void product_component::process(const double input[])
			{
				m_output[0] = input[0] * input[1];
			}
			
			//--

			product_factory::product_factory()
				: plugin_factory("Product", "Product", product_component_id)
			{
			}

			abstract_sound_component * product_factory::create_sound_component(data_source & source, const unsigned int channel_count)
			{
				return new product_component(channel_count);
			}

			abstract_sound_component * product_factory::create_sound_component(const abstract_form_answer& answer_form, const unsigned int channel_count)
			{
				return new product_component(channel_count);
			}

		} /* Builtin */

	} /* Sound */

} /* Gammou */