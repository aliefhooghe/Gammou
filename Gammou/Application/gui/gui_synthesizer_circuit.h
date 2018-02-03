#ifndef GUI_SYNTHESIZER_CIRCUITS_H_
#define GUI_SYNTHESIZER_CIRCUITS_H_

#include "../../Synthesizer/plugin_management/main_factory.h"
#include "../persistence/synthesizer_persistence.h"
#include "gui_sound_component.h"

namespace Gammou {

	namespace Gui {

		class abstract_gui_synthesizer_circuit : public abstract_gui_component_map {

		public:
			abstract_gui_synthesizer_circuit(
				Sound::main_factory *main_factory,
				const unsigned int components_channel_count,
				Sound::synthesizer *synthesizer, 
				std::mutex *synthesizer_mutex,
				unsigned int x, 
				const unsigned int y, 
				const unsigned int width, 
				const unsigned int height,
				const View::color background = GuiProperties::background);

			abstract_gui_synthesizer_circuit(
				Sound::main_factory *main_factory,
				const unsigned int component_channel_count,
				Sound::synthesizer *synthesizer, 
				std::mutex *synthesizer_mutex,
				const View::rectangle& rect, 
				const View::color background = GuiProperties::background);

			virtual ~abstract_gui_synthesizer_circuit() {}

			virtual bool on_mouse_dbl_click(const int x, const int y) override;
			void select_component_creation_factory_id(const unsigned int factory_id);

			bool save_state(Sound::data_sink& data);
			bool load_state(Sound::data_source& data);
		protected:
			// Warning : Synthesizer Mutex is alredy lock when add_sound_component_to_frame is called
			virtual void add_sound_component_to_frame(Sound::abstract_sound_component *sound_component) = 0;

			virtual uint8_t get_component_internal_id(abstract_gui_component *component) = 0;
			virtual abstract_gui_component *gui_component_by_internal_id(const uint8_t internal_id) = 0;

			void lock_circuit() { m_synthesizer_mutex->lock(); }
			void unlock_circuit() { m_synthesizer_mutex->unlock(); }

			Sound::synthesizer *const m_synthesizer;

		private:
			void save_component(Sound::data_sink& data, abstract_gui_component *component);
			abstract_gui_component *load_component(Sound::data_source& data); // Add the process_component on the frame
			void save_link(Sound::data_sink& data, const unsigned int src_record_id, const unsigned int output_id,
				const unsigned int dst_record_id, const unsigned int input_id);

			void reset_content();
			//----------------
			
			std::mutex *const m_synthesizer_mutex;
			Sound::main_factory *const m_main_factory;

			const unsigned int m_components_channel_count;
			unsigned int m_creation_factory_id;
		};


	} /* Gui */

} /* Gammou */


#endif