#ifndef ABSTRACT_PLUGIN_FACTORY_H_
#define ABSTRACT_PLUGIN_FACTORY_H_

#include "request_form.h"
#include "../sound_component/sound_component.h"

namespace Gammou {

    namespace Sound {

        class abstract_plugin_factory{

        public:
			abstract_plugin_factory(const unsigned int factory_id);
			virtual ~abstract_plugin_factory() {}

			const request_form_descriptor& get_request_form() const;

			// todo : peut etre du unique ptr ??
			virtual abstract_sound_component *create_module(const answer_form_descriptor& answer) =0;

		protected:
			void stamp_sound_component(abstract_sound_component *component) const;

        private:
			const unsigned int m_factory_id;
			request_form_descriptor m_request_form;
        };
		
    } /* Sound */

} /* Gammou */



#endif