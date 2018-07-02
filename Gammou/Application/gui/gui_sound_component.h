#ifndef GUI_SOUND_COMPONENT_H_
#define GUI_SOUND_COMPONENT_H_

#include <map>
#include <memory>
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
				std::unique_ptr<Sound::abstract_sound_component> && component,
				const unsigned int x, const unsigned int y);
			virtual ~gui_sound_component();
			
			// FACTORY stuff
			unsigned int get_sound_component_factory_id() const override;
			unsigned int save_sound_component_state(Sound::data_sink& data) override;

			Sound::abstract_sound_component& get_sound_component();
		protected:
			Process::abstract_component<double> *get_component() const override;
			std::unique_ptr<Sound::abstract_sound_component> m_sound_component;
		};

	}	/* Gui */
} /* Gammou */


#endif