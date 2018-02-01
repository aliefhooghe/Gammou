#ifndef GUI_MASTER_CIRCUIT_H_
#define GUI_MASTER_CIRCUIT_H_

#include "gui_synthesizer_circuit.h"

namespace Gammou {

	namespace Gui {

		class gui_master_circuit : public abstract_gui_synthesizer_circuit {

			enum internal_component_id : uint8_t
			{
				INPUT,
				OUTPUT,
				PARAMETERS,
				POLY_IN,
				POLY_OUT
			};

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
			uint8_t get_component_internal_id(abstract_gui_component *component) override;

		private:
			void add_internal_components(std::mutex *synthesizer_mutex);
		};


	} /* Gui */
} /* Gammou */


#endif