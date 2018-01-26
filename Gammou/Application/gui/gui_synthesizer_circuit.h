#ifndef GUI_SYNTHESIZER_CIRCUITS_H_
#define GUI_SYNTHESIZER_CIRCUITS_H_

#include "gui_sound_component.h"

namespace Gammou {

	namespace Gui {

		class gui_master_circuit : public abstract_gui_component_map {

		public:
			gui_master_circuit(Sound::synthesizer *synthesizer, std::mutex *synthesizer_mutex,
				unsigned int x, const unsigned int y, const unsigned int width, const unsigned int height, 
				const View::color background = View::cl_dimgray);
			gui_master_circuit(Sound::synthesizer *synthesizer, std::mutex *synthesizer_mutex, 
				const View::rectangle& rect, const View::color background = View::cl_dimgray);
			
			virtual ~gui_master_circuit() {}

		private:
			void add_internal_components(std::mutex *synthesizer_mutex);

			Sound::synthesizer *const m_synthesizer;
		};

	} /* Gui */

} /* Gammou */


#endif