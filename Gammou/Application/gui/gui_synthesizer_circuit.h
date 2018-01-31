#ifndef GUI_SYNTHESIZER_CIRCUITS_H_
#define GUI_SYNTHESIZER_CIRCUITS_H_

#include "../../Synthesizer/plugin_management/main_factory.h"
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

		protected:
			// Warning : Synthesizer Mutex is alredy lock 
			virtual void add_sound_component_to_frame(Sound::abstract_sound_component *sound_component) = 0;

			void lock_circuit() { m_synthesizer_mutex->lock(); }
			void unlock_circuit() { m_synthesizer_mutex->unlock(); }

			Sound::synthesizer *const m_synthesizer;

		private:
			std::mutex *const m_synthesizer_mutex;
			Sound::main_factory *const m_main_factory;

			const unsigned int m_components_channel_count;
			unsigned int m_creation_factory_id;
		};


		class gui_master_circuit : public abstract_gui_synthesizer_circuit {

		public:
			gui_master_circuit(
				Sound::main_factory *main_factory,
				Sound::synthesizer *synthesizer, 
				std::mutex *synthesizer_mutex,
				unsigned int x, 
				const unsigned int y, 
				const unsigned int width, 
				const unsigned int height, 
				const View::color background = GuiProperties::background);

			gui_master_circuit(
				Sound::main_factory *main_factory,
				Sound::synthesizer *synthesizer, 
				std::mutex *synthesizer_mutex, 
				const View::rectangle& rect, 
				const View::color background = GuiProperties::background);
			
			virtual ~gui_master_circuit() {}

		protected:
			void add_sound_component_to_frame(Sound::abstract_sound_component *sound_component) override;

		private:
			void add_internal_components(std::mutex *synthesizer_mutex);
		};

		
		class gui_polyphonic_circuit : public abstract_gui_synthesizer_circuit {

		public:
			gui_polyphonic_circuit(
				Sound::main_factory *main_factory,
				Sound::synthesizer *synthesizer, 
				std::mutex *synthesizer_mutex,
				unsigned int x, 
				const unsigned int y, 
				const unsigned int width, 
				const unsigned height,
				const View::color background = GuiProperties::background);

			gui_polyphonic_circuit(
				Sound::main_factory *main_factory,
				Sound::synthesizer *synthesizer, 
				std::mutex *synthesizer_mutex,
				const View::rectangle& rect, 
				const View::color background = GuiProperties::background);

			virtual ~gui_polyphonic_circuit() {}

		protected:
			void add_sound_component_to_frame(Sound::abstract_sound_component *sound_component) override;

		private:
			void add_internal_components(std::mutex *synthesizer_mutex);

		};
		

	} /* Gui */

} /* Gammou */


#endif