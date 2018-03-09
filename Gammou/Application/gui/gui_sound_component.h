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
			gui_sound_component(
				Sound::abstract_sound_component *sound_component, 
			/*	std::mutex *synthesizer_mutex,*/
				const unsigned int x, const unsigned int y);
			virtual ~gui_sound_component();
			
			// FACTORY stuff
			unsigned int get_sound_component_factory_id() const override;
			unsigned int save_sound_component_state(Sound::data_sink& data) override;

		protected:
			Process::abstract_component<double> *get_component() const override;
			Sound::abstract_sound_component *const m_sound_component;
		};


	}	/* Gui */
} /* Gammou */


#endif