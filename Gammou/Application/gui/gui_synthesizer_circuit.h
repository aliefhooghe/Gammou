#ifndef GUI_SYNTHESIZER_CIRCUITS_H_
#define GUI_SYNTHESIZER_CIRCUITS_H_

#include "gui_sound_component.h"

namespace Gammou {

	namespace Gui {

		class abstract_gui_synthesizer_circuit : public abstract_gui_component_map {

		public:
			abstract_gui_synthesizer_circuit(Sound::synthesizer *synthesizer, std::mutex *synthesizer_mutex,
				unsigned int x, const unsigned int y, const unsigned int width, const unsigned int height,
				const View::color background);
			abstract_gui_synthesizer_circuit(Sound::synthesizer *synthesizer, std::mutex *synthesizer_mutex,
				const View::rectangle& rect, const View::color background);

			virtual ~abstract_gui_synthesizer_circuit() {}

		protected:
			Sound::synthesizer *const m_synthesizer;
		};


		class gui_master_circuit : public abstract_gui_synthesizer_circuit {

		public:
			gui_master_circuit(Sound::synthesizer *synthesizer, std::mutex *synthesizer_mutex,
				unsigned int x, const unsigned int y, const unsigned int width, const unsigned int height, 
				const View::color background = View::cl_dimgray);
			gui_master_circuit(Sound::synthesizer *synthesizer, std::mutex *synthesizer_mutex, 
				const View::rectangle& rect, const View::color background = View::cl_dimgray);
			
			virtual ~gui_master_circuit() {}

		private:
			void add_internal_components(std::mutex *synthesizer_mutex);
		};

		
		class gui_polyphonic_circuit : public abstract_gui_synthesizer_circuit {

		public:
			gui_polyphonic_circuit(Sound::synthesizer *synthesizer, std::mutex *synthesizer_mutex,
				unsigned int x, const unsigned int y, const unsigned int width, const unsigned height,
				const View::color background = View::cl_gray);
			gui_polyphonic_circuit(Sound::synthesizer *synthesizer, std::mutex *synthesizer_mutex,
				const View::rectangle& rect, const View::color background  = View::cl_gray);

			virtual ~gui_polyphonic_circuit() {}

		private:
			void add_internal_components(std::mutex *synthesizer_mutex);

		};
		

	} /* Gui */

} /* Gammou */


#endif