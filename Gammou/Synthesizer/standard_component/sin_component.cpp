
#include "sin_component.h"

namespace Gammou {

	namespace Sound {


		sin_component::sin_component(const unsigned int channel_count)
			: sound_component("Sin", 2, 1, channel_count),
			m_frequence_integral(this)
		{
		}

		void sin_component::process(const double input[])
		{
			m_output[0] = sin(m_frequence_integral + input[1]);
			m_frequence_integral += (input[0] * get_sample_duration());
		}

		void sin_component::initialize_process()
		{
			m_frequence_integral = 0.0;
		}



		sin_factory::sin_factory()
			: abstract_plugin_factory("Sin", "C'est un sinus", 42) // TODO id !!!!
		{
		}

		abstract_sound_component * sin_factory::create_sound_component(data_source & source, const unsigned int channel_count)
		{
			return new sin_component(channel_count);
		}

		abstract_sound_component * sin_factory::create_sound_component(const answer_form_descriptor & answer_form, const unsigned int channel_count)
		{
			return new sin_component(channel_count);
		}

	} /* Sound */

} /* Gammou */