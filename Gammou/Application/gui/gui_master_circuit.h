#ifndef GUI_MASTER_CIRCUIT_H_
#define GUI_MASTER_CIRCUIT_H_

#include "gui_synthesizer_circuit.h"

namespace Gammou {

	namespace Gui {

		class gui_master_circuit : public abstract_gui_synthesizer_circuit {

			enum internal_component_id : uint32_t
			{
				INPUT = 0u,
				OUTPUT,
				PARAMETERS,
				POLY_IN,
				POLY_OUT
			};

		public:

			gui_master_circuit(
                gui_component_main_factory& complete_component_factory,
				Sound::synthesizer *synthesizer,
				std::mutex *synthesizer_mutex,
                const int x,
                const int y,
				const unsigned int width,
				const unsigned int height,
				const View::color background = GuiProperties::background);

			gui_master_circuit(
                gui_component_main_factory& complete_component_factory,
				Sound::synthesizer *synthesizer,
				std::mutex *synthesizer_mutex,
				const View::rectangle& rect,
				const View::color background = GuiProperties::background);

			virtual ~gui_master_circuit() {}

		protected:
			void add_sound_component_to_frame(Sound::abstract_sound_component *sound_component) override;
			abstract_gui_component *gui_component_by_internal_id(const uint32_t id) override;

		private:
			void add_internal_components(std::mutex *synthesizer_mutex);

			abstract_gui_component *m_parameter_input;
			abstract_gui_component *m_polyphonic_input;
			abstract_gui_component *m_polyphonic_output;
			abstract_gui_component *m_main_input;
			abstract_gui_component *m_main_output;
		};


	} /* Gui */
} /* Gammou */


#endif
