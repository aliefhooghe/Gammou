
#include "gui_synthesizer_circuit.h"

namespace Gammou {

	namespace Gui {
		gui_master_circuit::gui_master_circuit(
			Sound::synthesizer * synthesizer, std::mutex *synthesizer_mutex, 
			const unsigned int x, const unsigned int y, const unsigned int width, const unsigned int height, const View::color background)
			: abstract_gui_component_map(synthesizer_mutex, x, y, width, height, background),
			m_synthesizer(synthesizer)
		{
			add_internal_components(synthesizer_mutex);
		}

		gui_master_circuit::gui_master_circuit(Sound::synthesizer * synthesizer, std::mutex *synthesizer_mutex,
			const View::rectangle & rect, const View::color background)
			: abstract_gui_component_map(synthesizer_mutex, rect, background),
			m_synthesizer(synthesizer)
		{
			add_internal_components(synthesizer_mutex);
		}

		void gui_master_circuit::add_internal_components(std::mutex *synthesizer_mutex)
		{
			add_gui_component(
				new default_gui_component(
					m_synthesizer->get_master_circuit_automation_input(), 
					synthesizer_mutex, 10, 10));
			add_gui_component(
				new default_gui_component(
					m_synthesizer->get_master_circuit_polyphonic_input(),
					synthesizer_mutex, 10, 10));
			add_gui_component(
				new default_gui_component(
					m_synthesizer->get_master_circuit_polyphonic_output() ,
					synthesizer_mutex, 10, 10));
			add_gui_component(
				new default_gui_component(
					m_synthesizer->get_master_main_input() ,
					synthesizer_mutex, 10, 10));
			add_gui_component(
				new default_gui_component(
					m_synthesizer->get_master_main_output() ,
					synthesizer_mutex, 10, 10));
		}
	} /* Gui */

} /* Gammou */
