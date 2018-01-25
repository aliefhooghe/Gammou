
#include "gui_sound_component.h"


namespace Gammou {

	namespace Gui {



		gui_sound_component::gui_sound_component(Sound::abstract_sound_component * sound_component, const unsigned int x, const unsigned int y)
			: abstract_gui_component(x, y, sound_component->get_input_count(), sound_component->get_output_count()),
					m_sound_component(sound_component)
		{
		}

		gui_sound_component::~gui_sound_component()
		{
			// PROBABLEMENT : delete sound_componnt
		}

		Process::abstract_component<double>* gui_sound_component::get_component() const
		{
			return m_sound_component;
		}




} /* Gui */

} /* Gammou */