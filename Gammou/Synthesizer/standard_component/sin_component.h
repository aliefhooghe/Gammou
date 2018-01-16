#ifndef SIN_COMPONENT_H_
#define SIN_COMPONENT_H_

#include "../plugin/abstract_plugin_factory.h"


namespace Gammou {

	namespace Sound {


		class sin_component : public sound_component {

		public:
			sin_component(const unsigned int channel_count);
			~sin_component() {}

			void process(const double input[]) override;
			void initialize_process() override;

		private:
			multi_channel_variable<double> m_frequence_integral;
		};


		class sin_factory : public abstract_plugin_factory {

		public:
			sin_factory();
			~sin_factory() {}

		protected:
			abstract_sound_component *create_sound_component(data_source& source, const unsigned int channel_count) override;
			abstract_sound_component *create_sound_component(const answer_form_descriptor& answer_form, const unsigned int channel_count) override;
		};


	} /* Sound */

} /* Gammou */


#endif