
#include "sum_component.h"

namespace Gammou {

	namespace Sound {

		namespace Builtin {

			sum_component::sum_component(const unsigned int channel_count)
				: sound_component("Sum", 1, 1, channel_count)
			{
			}

			sum_component::~sum_component()
			{
				DEBUG_PRINT("DEBUG Component DTOR\n");
			}

			void sum_component::process(const double input[])
			{
				const unsigned int nb = get_input_count();
				double ret = 0.0;

				for (unsigned int i = 0; i < nb; ++i)
					ret += input[i];

				m_output[0] = ret;
			}

			void sum_component::initialize_process()
			{
				const unsigned int channel = get_current_working_channel();
			}

			void sum_component::on_input_connection(const unsigned int input_id)
			{
				if (free_input_count() == 0)
					push_input();
			}

			void sum_component::on_input_deconnection(const unsigned int input_id)
			{	
				unsigned int ic = get_input_count(); 
				unsigned int free_input = free_input_count();

				while (free_input > 1 && !(is_input_connected(ic - 1))) {
					free_input--;
					ic--;
					pop_input();
				}	
			}

			unsigned int sum_component::free_input_count()
			{
				const unsigned int ic = get_input_count();
				unsigned int ret = 0;

				for (unsigned int i = 0; i < ic; ++i) {
					if (!is_input_connected(i))
						ret++;
				}

				return ret;
			}

			////-------------------------

			sum_component_factory::sum_component_factory()
				: plugin_factory("Sum", "Sum", sum_component_id)
			{
			}

			abstract_sound_component * sum_component_factory::create_sound_component(data_source & source, const unsigned int channel_count)
			{
				return new sum_component(channel_count);
			}

			abstract_sound_component * sum_component_factory::create_sound_component(const abstract_form_answer& answer_form, const unsigned int channel_count)
			{
				return new sum_component(channel_count);
			}


		} /* Builtin */

	} /* Sound */

} /* Gammou */
