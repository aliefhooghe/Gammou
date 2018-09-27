#ifndef GUI_COMPONENT_MAIN_FACTORY_H
#define GUI_COMPONENT_MAIN_FACTORY_H

#include "gui_sound_component.h"
#include "../../Synthesizer/plugin_management/main_factory.h"
#include "abstract_gui_component_factory.h" // complete factory (todo rename)

namespace Gammou {

	namespace Gui {
		

		class gui_component_main_factory {

		public:
			gui_component_main_factory();
			~gui_component_main_factory();

			const std::string& get_factory_name(const unsigned int factory_id);
            const std::string& get_factory_category(const unsigned int factory_id);

			const Sound::request_form& get_plugin_request_form(const unsigned int factory_id);

            std::unique_ptr<gui_sound_component> get_new_gui_component(
					const unsigned int factory_id, 
					const int x, const int y, 
					Sound::data_input_stream& data, 
					const unsigned int channel_count);
            std::unique_ptr<gui_sound_component> get_new_gui_component(
				const unsigned int factory_id,
				const int x, const int y,
				const Sound::answer_form& answer_form,
				const unsigned int channel_count);

			unsigned int load_plugin_factory(const std::string& file_path);
            void add_plugin_factory(std::unique_ptr<Sound::abstract_plugin_factory> && factory);
            void add_complete_factory(std::unique_ptr<abstract_gui_component_factory> && factory);
			bool check_factory_presence(const unsigned int factory_id) const;

            std::unique_ptr<abstract_gui_component> clone_component(
                    abstract_gui_component& component,
                    const int gui_x, const int gui_y,
                    const unsigned int channel_count);

		protected:
			std::unique_ptr<gui_sound_component> 
				create_default_complete_component(
					const int x, const int y, 
					Sound::abstract_sound_component* sound_component);

		private:
			abstract_gui_component_factory *factory_by_id(const unsigned int factory_id);

            std::map<
                unsigned int,
                std::unique_ptr<abstract_gui_component_factory> > m_complete_component_factories;
			Sound::main_factory m_main_factory;

		};
		
	}

}

#endif
