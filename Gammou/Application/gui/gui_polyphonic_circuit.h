#ifndef GUI_POLYPHONIC_CIRCUIT_H_
#define GUI_POLYPHONIC_CIRCUIT_H_

#include "gui_synthesizer_circuit.h"

namespace Gammou {

	namespace Gui {


		class gui_polyphonic_circuit : public abstract_gui_synthesizer_circuit {

			enum internal_component_id : uint8_t
			{
				MASTER_IN,
				MASTER_OUT,
				PARAMETERS,
				MIDI
			};

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
			uint8_t get_component_internal_id(abstract_gui_component *component) override;

		private:
			void add_internal_components(std::mutex *synthesizer_mutex);

		};


	} /* Gui */
} /* Gammou */


#endif