
#include "sin_component.h"

namespace Gammou {

	namespace Sound {

		namespace Builtin {

			sin_component::sin_component(const unsigned int channel_count)
				: sound_component("Sine", 2, 1, channel_count),
				m_frequence_integral(this)
			{
				set_input_name("Pitch", 0);
				set_input_name("Phase", 1);
			}

			sin_component::~sin_component()
			{
			
			}

			void sin_component::process(const double input[])
			{
				m_output[0] = std::sin(6.28318530718 * m_frequence_integral + input[1]);
				m_frequence_integral += (input[0] * get_sample_duration());
			}

			void sin_component::initialize_process()
			{
				m_frequence_integral = 0.0;
			}

			sin_factory::sin_factory()
				: plugin_factory("Sine", "Sinus oscilator", sin_component_id)
			{
			}

			abstract_sound_component * sin_factory::create_sound_component(data_source & source, const unsigned int channel_count)
			{
				return new sin_component(channel_count);
			}

			abstract_sound_component * sin_factory::create_sound_component(const abstract_form_answer& answer_form, const unsigned int channel_count)
			{
				return new sin_component(channel_count);
			}

		}

	} /* Sound */

} /* Gammou */