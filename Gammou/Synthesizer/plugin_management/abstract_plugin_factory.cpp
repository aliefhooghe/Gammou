
#include "abstract_plugin_factory.h"

namespace Gammou {

	namespace Sound {

		abstract_plugin_factory::abstract_plugin_factory(const std::string & name, const std::string & category, unsigned int factory_id)
			: m_name(name), m_category(m_name), m_factory_id(factory_id)
		{
		}

		const std::string & abstract_plugin_factory::get_name() const
		{
			return m_name;
		}

		const std::string & abstract_plugin_factory::get_category() const
		{
			return m_category;
		}

		unsigned int abstract_plugin_factory::get_factory_id() const
		{
			return m_factory_id;
		}

		const request_form & abstract_plugin_factory::get_request_form()
		{
			if (!m_plugin_request_form)
				m_plugin_request_form = create_plugin_request_form();
			return (*m_plugin_request_form);
		}

		void abstract_plugin_factory::stamp_sound_component(abstract_sound_component * component) const
		{
			if (component == nullptr)
				throw std::domain_error("Cannot stamp null component");
			else if (component->m_factory_id != NO_FACTORY && component->m_factory_id != m_factory_id)
				throw std::domain_error("Sound component is already stamped");
			else
				component->m_factory_id = m_factory_id;
		}

		std::unique_ptr<request_form> abstract_plugin_factory::create_plugin_request_form()
		{
			return std::make_unique<request_form>(empty_request_form{});
		}

		///////////////

		plugin_factory::plugin_factory(const std::string& name, const std::string& category, const unsigned int factory_id)
			: abstract_plugin_factory(name, category, factory_id)
		{
			
		}

		abstract_sound_component * plugin_factory::get_new_sound_component(data_input_stream & source, const unsigned int channel_count)
		{
			abstract_sound_component *component = create_sound_component(source, channel_count);
			stamp_sound_component(component);	//	set factory ID
			return component;
		}

		abstract_sound_component * plugin_factory::get_new_sound_component(const answer_form& answer, const unsigned int channel_count)
		{
			abstract_sound_component *component = create_sound_component(answer, channel_count);
			stamp_sound_component(component);
			return component;
		}

		void plugin_factory::delete_sound_component(abstract_sound_component * component) const
		{
			if( component != nullptr)
				delete component;
		}

	} /* Sound */

} /* Gammou */