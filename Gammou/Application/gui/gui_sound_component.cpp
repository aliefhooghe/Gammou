
#include "gui_sound_component.h"


namespace Gammou {

	namespace Gui {



		gui_sound_component::gui_sound_component(
			std::unique_ptr<Sound::abstract_sound_component> && sound_component, 
			const unsigned int x, const unsigned int y)
			: abstract_gui_component(
			 x, y, sound_component->get_input_count(), sound_component->get_output_count()),
					m_sound_component(std::move(sound_component))
		{
		}

		gui_sound_component::~gui_sound_component()
		{
		}

		unsigned int gui_sound_component::get_sound_component_factory_id() const
		{
			return m_sound_component->get_factory_id();
		}

		unsigned int gui_sound_component::save_sound_component_state(Sound::data_sink & data)
		{
			return m_sound_component->save_state(data);
		}

		Sound::abstract_sound_component & gui_sound_component::get_sound_component()
		{
			return *m_sound_component;
		}

		Process::abstract_component<double>* gui_sound_component::get_component() const
		{
			return m_sound_component.get();
		}




} /* Gui */

} /* Gammou */