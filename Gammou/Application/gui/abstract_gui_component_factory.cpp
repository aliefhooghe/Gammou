
#include "abstract_gui_component_factory.h"

namespace Gammou {

	namespace Gui {

		complete_component_factory::complete_component_factory(const std::string & name, const std::string & description, unsigned int factory_id)
			: abstract_plugin_factory(name, description, factory_id)
		{
		}
		
		void complete_component_factory::delete_sound_component(Sound::abstract_sound_component * component) const
		{
			if (component != nullptr)
				delete component;
		}
	} /* Gui */
} /* Gammou */