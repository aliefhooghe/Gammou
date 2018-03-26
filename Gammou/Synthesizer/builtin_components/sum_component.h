#ifndef DEBUG_COMPONENT_H_
#define DEBUG_COMPONENT_H_

#include "../../debug.h"
#include "../plugin_management/abstract_plugin_factory.h"

#include "builtin_components_ids.h"

namespace Gammou {

	namespace Sound {

		namespace Builtin {

			class sum_component : public sound_component {

			public:
				sum_component(const unsigned int channel_count);
				~sum_component();

				void process(const double input[]) override;
				void initialize_process() override;
				void on_input_connection(const unsigned int input_id) override;
				void on_input_deconnection(const unsigned int input_id) override;
			private:
				unsigned int free_input_count();

			};

			class sum_component_factory : public plugin_factory {

			public:
				sum_component_factory();
				~sum_component_factory() {}

			protected:
				abstract_sound_component *create_sound_component(data_source& source, const unsigned int channel_count) override;
				abstract_sound_component *create_sound_component(const abstract_form_answer& answer_form, const unsigned int channel_count) override;
			};

		} /* Builtin */

	} /* Sound */

} /* Gammou */

#endif