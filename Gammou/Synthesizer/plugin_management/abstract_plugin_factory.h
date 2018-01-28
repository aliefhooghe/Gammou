#ifndef ABSTRACT_PLUGIN_FACTORY_H_
#define ABSTRACT_PLUGIN_FACTORY_H_

#include <string>

#include "request_form.h"
#include "../sound_component/sound_component.h"
#include "data_stream.h"

namespace Gammou {

    namespace Sound {

        class abstract_plugin_factory{

        public:
			abstract_plugin_factory(const std::string& name, const std::string& description, unsigned int factory_id);
			virtual ~abstract_plugin_factory() {}

			const std::string& get_name() const;
			const std::string& get_description() const;
			const request_form_descriptor& get_request_form() const;
			
			abstract_sound_component *get_new_sound_component(data_source& source, const unsigned int channel_count);
			abstract_sound_component *get_new_sound_component(const answer_form_descriptor& answer, const unsigned int channel_count);

			unsigned int get_factory_id() const;
			void delete_sound_component(abstract_sound_component*component) const;

		protected:
			virtual abstract_sound_component *create_sound_component(data_source& source, const unsigned int channel_count) = 0;
			virtual abstract_sound_component *create_sound_component(const answer_form_descriptor& answer_form, const unsigned int channel_count) = 0;

		private:
			void stamp_sound_component(abstract_sound_component *component) const;

			const std::string m_name;
			const std::string m_description;
			const unsigned int m_factory_id;

			request_form_descriptor m_request_form;
        };
		
    } /* Sound */

} /* Gammou */



#endif