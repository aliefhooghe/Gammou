#ifndef ABSTRACT_COMPLETE_COMPONENT_FACTORY_H
#define ABSTRACT_COMPLETE_COMPONENT_FACTORY_H

#include <memory>
#include "gui_sound_component.h"
#include "../../Synthesizer/plugin_management/abstract_plugin_factory.h"

namespace Gammou {
	
	namespace Gui {
										// protected allow use of stamp of child class
		class abstract_gui_component_factory : protected Sound::abstract_plugin_factory {

		public:
			abstract_gui_component_factory(
				const std::string& name, 
				const std::string& category, 
				unsigned int factory_id);
            abstract_gui_component_factory(abstract_gui_component_factory&) = delete;
            abstract_gui_component_factory(abstract_gui_component_factory&&) = delete;
			virtual ~abstract_gui_component_factory() {}

            void delete_sound_component(Sound::abstract_sound_component *component) const override; // ??
			
			using Sound::abstract_plugin_factory::get_factory_id;
			using Sound::abstract_plugin_factory::get_name;
			using Sound::abstract_plugin_factory::get_category;
			using Sound::abstract_plugin_factory::get_request_form;

            virtual std::unique_ptr<gui_sound_component> create_gui_component(
					const int x, const int y, 
					Sound::data_input_stream& source,
					 const unsigned int channel_count) = 0;
            virtual std::unique_ptr<gui_sound_component> create_gui_component(
					const int x, const int y, 
					const Sound::answer_form& answers, 
					const unsigned int channel_count) = 0;
		private:
			Sound::abstract_sound_component *get_new_sound_component(
                Sound::data_input_stream&,
                const unsigned int) override { return nullptr;  } // stubs
			Sound::abstract_sound_component *get_new_sound_component(
                const Sound::answer_form&,
                const unsigned int) override { return nullptr; }
		};
		
	}
	
}

#endif
