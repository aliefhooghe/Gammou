
#include "gui_component_main_factory.h"

namespace Gammou {

	namespace Gui {

		gui_component_main_factory::gui_component_main_factory()
		{
		}

		gui_component_main_factory::~gui_component_main_factory()
		{
		}

		const std::string & gui_component_main_factory::get_factory_name(const unsigned int factory_id)
		{
			return m_main_factory.get_factory_name(factory_id);
		}

		const std::string & gui_component_main_factory::get_factory_description(const unsigned int factory_id)
		{
			return m_main_factory.get_factory_description(factory_id);
		}

		const Sound::abstract_request_form & gui_component_main_factory::get_plugin_request_form(const unsigned int factory_id)
		{
			return m_main_factory.get_plugin_request_form(factory_id);
		}

		gui_component_main_factory::complete_component
			gui_component_main_factory::get_new_complete_component(const unsigned int factory_id, const int x, const int y, Sound::data_source & data, const unsigned int channel_count)
		{
			
			if (m_main_factory.check_factory_presence(factory_id)) {
				// Todo smart ptr
				Sound::abstract_sound_component *sound_component = m_main_factory.get_new_sound_component(factory_id, data, channel_count);
				// default gui
				return create_default_complete_component(x, y, sound_component);
			}
			else {
				throw std::runtime_error("NOT IMPLEMENTED\n");
			}
		}

		gui_component_main_factory::complete_component 
			gui_component_main_factory::get_new_complete_component(const unsigned int factory_id, const int x, const int y, const Sound::abstract_form_answer & answer_form, const unsigned int channel_count)
		{
			if (m_main_factory.check_factory_presence(factory_id)) {
				// Todo smart ptr
				Sound::abstract_sound_component *sound_component = m_main_factory.get_new_sound_component(factory_id, answer_form, channel_count);
				// default gui
				return create_default_complete_component(x, y, sound_component);
			}
			else {
				throw std::runtime_error("NOT IMPLEMENTED\n");
			}
		}

		unsigned int gui_component_main_factory::load_factory(const std::string & file_path)
		{
			return m_main_factory.load_factory(file_path);
		}

		void gui_component_main_factory::register_factory(Sound::abstract_plugin_factory * factory)
		{
			return m_main_factory.register_factory(factory);
		}

		bool gui_component_main_factory::check_factory_presence(const unsigned int factory_id) const
		{
			return m_main_factory.check_factory_presence(factory_id);
		}

		gui_component_main_factory::complete_component 
			gui_component_main_factory::create_default_complete_component(const int x, const int y, Sound::abstract_sound_component * sound_component)
		{
			gui_sound_component *gui_component = new gui_sound_component(sound_component, x, y);
			return std::make_pair(gui_component, sound_component);
		}
	}

} /* Gammou */