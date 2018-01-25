#ifndef GUI_SOUND_COMPONENT_H_
#define GUI_SOUND_COMPONENT_H_

#include <map>
#include "view.h"
#include "synthesizer.h"
#include "gui_component.h"

namespace Gammou {

	namespace Gui {

		/*
		*
		*/

		class gui_sound_component : public abstract_gui_component {

		public:
			gui_sound_component(Sound::abstract_sound_component *sound_component, const unsigned int x, const unsigned int y);
			virtual ~gui_sound_component();
			
			// FACTORY STUFF TO DO

		protected:
			Process::abstract_component<double> *get_component() const override;
			Sound::abstract_sound_component *const m_sound_component;
		};


	}	/* Gui */
} /* Gammou */


#endif