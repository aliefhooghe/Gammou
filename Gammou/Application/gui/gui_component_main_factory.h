#ifndef GUI_COMPONENT_MAIN_FACTORY_H
#define GUI_COMPONENT_MAIN_FACTORY_H

#include "gui_sound_component.h"
#include "../../Synthesizer/plugin_management/main_factory.h"
#include "abstract_gui_component_factory.h" // complete factory (todo rename)

namespace Gammou {

	namespace Gui {
		

		class gui_component_main_factory {

		public:
			typedef std::pair<abstract_gui_component*, Sound::abstract_sound_component*> complete_component;

			gui_component_main_factory();
			~gui_component_main_factory();

			const std::string& get_factory_name(const unsigned int factory_id);
			const std::string& get_factory_description(const unsigned int factory_id);

			const Sound::abstract_request_form& get_plugin_request_form(const unsigned int factory_id);

			complete_component get_new_complete_component(const unsigned int factory_id, const int x, const int y, Sound::data_source& data, const unsigned int channel_count);
			complete_component get_new_complete_component(const unsigned int factory_id, const int x, const int y,  const Sound::abstract_form_answer& answer_form, const unsigned int channel_count);

			unsigned int load_plugin_factory(const std::string& file_path);
			void register_plugin_factory(Sound::abstract_plugin_factory *factory);
			void register_complete_factory(abstract_gui_component_factory *factory);
			bool check_factory_presence(const unsigned int factory_id) const;

		protected:
			complete_component create_default_complete_component(const int x, const int y, Sound::abstract_sound_component* sound_component);

		private:
			abstract_gui_component_factory *factory_by_id(const unsigned int factory_id);

			std::map<unsigned int, abstract_gui_component_factory*> m_complete_component_factories;
			Sound::main_factory m_main_factory;

		};
		
	}

}

#endif