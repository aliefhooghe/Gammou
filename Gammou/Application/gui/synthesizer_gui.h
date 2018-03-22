
#ifndef GAMMOU_SYNTHESIZER_GUI_H_
#define GAMMOU_SYNTHESIZER_GUI_H_


#include <mutex>

#include "../../debug.h"
#include "view.h"
#include "gui_sound_component.h"
#include "gui_master_circuit.h"
#include "gui_polyphonic_circuit.h"

#include "gui_component_main_factory.h"
#include "../../Synthesizer/builtin_components.h"

#include "control_component/knob_component.h"


#define GAMMOU_SYNTHESIZER_CHANNEL_COUNT 128

namespace Gammou {

	namespace Gui {

		class synthesizer_gui : public View::generic_window {

		public:
			synthesizer_gui(Sound::synthesizer *synthesizer, std::mutex *synthesizer_mutex);
			~synthesizer_gui();
			
			// Persistence
			bool save_state(Sound::data_sink& data);
			bool load_state(Sound::data_source& data);

		private:
			void add_plugin_factory(Sound::abstract_plugin_factory *factory);
			void add_control_factory(complete_component_factory *factory);
			void load_plugin_factory(const std::string& path);

			void init_main_factory(); // Load all built-in and plug in component

			//	UI Widgets (All freed by their panels)
			View::list_box *m_plugin_list_box;
			gui_master_circuit *m_gui_master_circuit;
			gui_polyphonic_circuit *m_gui_polyphonic_circuit;
			View::knob *m_master_volume;

			// Factory
			std::vector<unsigned int> m_factory_ids;
			gui_component_main_factory m_complete_component_factory;
			
			// -----
			unsigned int page_id;
		};

	}
}

#endif