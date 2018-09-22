#ifndef GUI_POLYPHONIC_CIRCUIT_H_
#define GUI_POLYPHONIC_CIRCUIT_H_

#include "gui_synthesizer_circuit.h"

namespace Gammou {

	namespace Gui {


		class gui_polyphonic_circuit : public abstract_gui_synthesizer_circuit {

			enum internal_component_id : uint32_t
			{
				MASTER_IN = 0u,
				MASTER_OUT,
				PARAMETERS,
				MIDI
			};

		public:
			gui_polyphonic_circuit(
                gui_component_main_factory& complete_component_factory,
				Sound::synthesizer *synthesizer,
				std::mutex *synthesizer_mutex,
                const int x,
                const int y,
				const unsigned int width,
				const unsigned height,
				const View::color background = GuiProperties::background);

			gui_polyphonic_circuit(
                gui_component_main_factory& complete_component_factory,
				Sound::synthesizer *synthesizer,
				std::mutex *synthesizer_mutex,
				const View::rectangle& rect,
				const View::color background = GuiProperties::background);

			virtual ~gui_polyphonic_circuit() {}

		protected:
			void add_sound_component_to_frame(Sound::abstract_sound_component *sound_component) override;
			abstract_gui_component *gui_component_by_internal_id(const uint32_t internal_id) override;

		private:
			void add_internal_components(std::mutex *synthesizer_mutex);

			abstract_gui_component *m_master_out;
			abstract_gui_component *m_master_in;
			abstract_gui_component *m_midi_input;
			abstract_gui_component *m_parameter_input;
		};


	} /* Gui */
} /* Gammou */


#endif
