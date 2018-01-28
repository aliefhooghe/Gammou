
#include "abstract_plugin_factory.h"

namespace Gammou {

	namespace Sound {



		abstract_plugin_factory::abstract_plugin_factory(const std::string& name, const std::string& description, const unsigned int factory_id)
			: m_name(name), m_description(m_name), m_factory_id(factory_id)
		{
		}

		const std::string & abstract_plugin_factory::get_name()
		{
			return m_name;
		}

		const std::string & abstract_plugin_factory::get_description()
		{
			return m_description;
		}

		const request_form_descriptor & abstract_plugin_factory::get_request_form() const
		{
			return m_request_form;
		}

		abstract_sound_component * abstract_plugin_factory::get_new_sound_component(data_source & source, const unsigned int channel_count)
		{
			abstract_sound_component *component = create_sound_component(source, channel_count);
			stamp_sound_component(component);	//	set factory ID
			return component;
		}

		abstract_sound_component * abstract_plugin_factory::get_new_sound_component(const answer_form_descriptor & answer, const unsigned int channel_count)
		{
			abstract_sound_component *component = create_sound_component(answer, channel_count);
			stamp_sound_component(component);
			return component;
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
		
		void abstract_plugin_factory::stamp_sound_component(abstract_sound_component * component) const
		{
			if(component == nullptr)
				throw std::domain_error("Cannot stamp null component");
			else if (component->m_factory_id == NO_FACTORY)
				component->m_factory_id = m_factory_id;
			else if (component->m_factory_id != m_factory_id)
				throw std::domain_error("Sound component is already stamped");
		}


	} /* Sound */

} /* Gammou */