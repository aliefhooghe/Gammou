#ifndef GAMMOU_FUNCTION_COMPONENT_H_
#define GAMMOU_FUNCTION_COMPONENT_H_

#include "../../debug.h"
#include "../plugin_management/abstract_plugin_factory.h"
#include "sound_processing_helper.h"

#include "builtin_components_ids.h"

#define MAKE_UNIQUE_FUNCTION_COMPONENT_FACTORY(func) \
                std::make_unique<Sound::function_factory<func> >(#func, Sound::func##_component_id)

namespace Gammou {

	namespace Sound {

		template<double func(double)>
		class function_component : public sound_component {

		public:
			function_component(
				const std::string& name,
				const unsigned int channel_count)
				: sound_component(name, 1, 1, channel_count)
			{}
			~function_component() {}

			void process(const double input[]) override
			{
				m_output[0] = func(input[0]);
			}
		};

		template<double func(double)>
		class function_factory :
			public plugin_factory
		{
		public:
			function_factory(const std::string& name, const unsigned int id)
				: plugin_factory
				(name, "Calculus", id)
			{}
			~function_factory() {}

		protected:
			virtual abstract_sound_component *create_sound_component(
				data_input_stream& source,
				const unsigned int channel_count) override
			{
				return new function_component<func>(get_name(), channel_count);
			}

			virtual abstract_sound_component *create_sound_component(
				const answer_form& answer_form,
				const unsigned int channel_count) override
			{
				return new function_component<func>(get_name(), channel_count);
			}
		};

	} /* Sound */

} /* Gammou */


#endif
