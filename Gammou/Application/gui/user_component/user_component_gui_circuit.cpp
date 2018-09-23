
#include "user_component_gui_circuit.h"
#include "user_sound_component.h"

namespace Gammou {

	namespace Gui {

		user_component_gui_circuit::user_component_gui_circuit(
			gui_component_main_factory& factory,
			user_sound_component& component,
			const int x,
			const int y,
			const unsigned int width,
			const unsigned int height,
			const View::color background)
		:	abstract_gui_circuit (
				factory,
				component.get_channel_count(),
				x, y, width, height, background),
			m_component(component)
        {
            add_internal_components();
        }

		user_component_gui_circuit::~user_component_gui_circuit()
		{}

		void user_component_gui_circuit::add_sound_component_to_frame(
					Sound::abstract_sound_component *sound_component)
		{
			m_component.add_sound_component(sound_component);
		}

		abstract_gui_component *user_component_gui_circuit::gui_component_by_internal_id(const uint32_t id)
		{
            switch (id)
			{

			case internal_component_id::INPUT:
				return m_input;
				break;

			case internal_component_id::OUTPUT:
				return m_output;
				break;

			default:
				throw std::domain_error("INVALID Internal Id\n");
				break;
			}
		}

		void user_component_gui_circuit::add_internal_components()
		{
			auto input =
					std::make_unique<internal_gui_component>(
						&(m_component.get_input()),
						internal_component_id::INPUT,
						10, 100);
			m_input = input.get();

			auto output =
					std::make_unique<internal_gui_component>(
						&(m_component.get_output()),
						internal_component_id::OUTPUT,
						250, 100);
			m_output = output.get();

			add_gui_component(std::move(input));
			add_gui_component(std::move(output));
		}


	}   /*  Gui */

} /* Gammou */
