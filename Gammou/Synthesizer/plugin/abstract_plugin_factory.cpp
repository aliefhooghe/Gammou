#include "abstract_plugin_factory.h"

namespace Gammou {

	namespace Sound {



		abstract_plugin_factory::abstract_plugin_factory(const unsigned int factory_id)
			: m_factory_id(factory_id)
		{
		}

		const request_form_descriptor & abstract_plugin_factory::get_request_form() const
		{
			return m_request_form;
		}

		unsigned int abstract_plugin_factory::get_factory_id() const
		{
			return m_factory_id;
		}

		void abstract_plugin_factory::delete_sound_component(abstract_sound_component * component) const
		{
			if( component != nullptr)
				delete component;
		}
		
		void Gammou::Sound::abstract_plugin_factory::stamp_sound_component(abstract_sound_component * component) const
		{
			if (component->m_factory_id == NO_FACTORY)
				component->m_factory_id = m_factory_id;
			else if (component->m_factory_id != m_factory_id)
				throw std::domain_error("Sound component is already stamped");
		}


	} /* Sound */

} /* Gammou */