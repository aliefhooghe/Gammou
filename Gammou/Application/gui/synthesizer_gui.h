
#ifndef GAMMOU_SYNTHESIZER_GUI_H_
#define GAMMOU_SYNTHESIZER_GUI_H_


#include <mutex>
#include <view.h>

#include "../../debug.h"
#include "gui_sound_component.h"
#include "gui_master_circuit.h"
#include "gui_polyphonic_circuit.h"

#include "gui_component_main_factory.h"

#include "control_component/knob_gui_component.h"
#include "control_component/slider_gui_component.h"
#include "control_component/integer_gui_component.h"

#include "user_component/user_gui_component.h"

#ifndef GAMMOU_PLUGINS_DIRECTORY_PATH
#error "GAMMOU_PLUGINS_DIRECTORY_PATH must be defined"
#endif

#define GAMMOU_SYNTHESIZER_CHANNEL_COUNT 128
#define GAMMOU_PARAMETER_INPUT_COUNT 16u

namespace Gammou {

	namespace Gui {

		class synthesizer_gui : public View::window_widget {

		public:
			synthesizer_gui(
				Sound::synthesizer *synthesizer, 
				std::mutex *synthesizer_mutex);
			~synthesizer_gui();
			
			// Persistence
			bool save_state(Sound::data_output_stream& data);
			bool load_state(Sound::data_input_stream& data);

		private:
            void add_plugin_factory(std::unique_ptr<Sound::abstract_plugin_factory> && factory);
            void add_control_factory(std::unique_ptr<abstract_gui_component_factory> && factory);
			void load_plugin_factory(const std::string& path);

			void init_main_factory(); // Load all built-in and plug in component

			//
			Sound::synthesizer& m_synthesizer;

			//	UI Widgets (All freed by their panels)
            View::list_box *m_plugin_list_box{};
            gui_master_circuit *m_gui_master_circuit{};
            gui_polyphonic_circuit *m_gui_polyphonic_circuit{};
            View::page_container* m_pages;
            View::knob *m_master_volume{};

            // Factory
            std::vector<unsigned int> m_factory_ids;
            gui_component_main_factory m_gui_component_factory;

			// -----
            unsigned int m_page_id;   //  Current page id
		};

	}
}

#endif
