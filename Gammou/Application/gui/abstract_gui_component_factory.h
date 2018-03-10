#ifndef ABSTRACT_COMPLETE_COMPONENT_FACTORY_H
#define ABSTRACT_COMPLETE_COMPONENT_FACTORY_H


#include "gui_component.h"
#include "../../Synthesizer/plugin_management/abstract_plugin_factory.h"

namespace Gammou {
	
	namespace Gui {
										// allow use of stamp
		class complete_component_factory : protected Sound::abstract_plugin_factory {

		public:
			typedef std::pair<abstract_gui_component*, Sound::abstract_sound_component*> complete_component;

			complete_component_factory(const std::string& name, const std::string& description, unsigned int factory_id);
			virtual ~complete_component_factory() {}

			void delete_sound_component(Sound::abstract_sound_component *component) const override;
			
			using Sound::abstract_plugin_factory::get_factory_id;
			using Sound::abstract_plugin_factory::get_name;
			using Sound::abstract_plugin_factory::get_description;
			using Sound::abstract_plugin_factory::get_request_form;

			virtual complete_component create_complete_component(const int x, const int y, Sound::data_source& source, const unsigned int channel_count) = 0;
			virtual complete_component create_complete_component(const int x, const int y, const Sound::abstract_form_answer& answer_form, const unsigned int channel_count) = 0;
		
		private:
			Sound::abstract_sound_component *get_new_sound_component(Sound::data_source& source, const unsigned int channel_count) { return nullptr;  } // stubs
			Sound::abstract_sound_component *get_new_sound_component(const Sound::abstract_form_answer& answer, const unsigned int channel_count) { return nullptr; }
		};
		
	}
	
}

#endif