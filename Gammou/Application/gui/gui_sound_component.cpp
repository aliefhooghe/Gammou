
#include "gui_sound_component.h"


namespace Gammou {

	namespace Gui {



		gui_sound_component::gui_sound_component(
			Sound::abstract_sound_component * sound_component, /*std::mutex *synthesizer_mutex,*/ const unsigned int x, const unsigned int y)
			: abstract_gui_component(
				/*synthesizer_mutex,*/ x, y, sound_component->get_input_count(), sound_component->get_output_count()),
					m_sound_component(sound_component)
		{
		}

		gui_sound_component::~gui_sound_component()
		{
			// TODO : Factory delete !
			delete m_sound_component;
		}

		unsigned int gui_sound_component::get_sound_component_factory_id() const
		{
			return m_sound_component->get_factory_id();
		}

		unsigned int gui_sound_component::save_sound_component_state(Sound::data_sink & data)
		{
			return m_sound_component->save_state(data);
		}

		Process::abstract_component<double>* gui_sound_component::get_component() const
		{
			return m_sound_component;
		}




} /* Gui */

} /* Gammou */