#ifndef GAMMOU_INTEGER_GUI_COMPONENT_H_
#define GAMMOU_INTEGER_GUI_COMPONENT_H_

#include "gui_sound_component.h"
#include "abstract_gui_component_factory.h"
#include "integer_control_sound_component.h"
#include "control_component_ids.h"

namespace Gammou {

	namespace Gui {

		class integer_gui_component : public gui_sound_component {

		public:
			integer_gui_component(
				integer_sound_component *integer,
				const unsigned int x, const unsigned int y);
			~integer_gui_component() {}

			bool on_mouse_dbl_click(const int x, const int y) override;

		private:
			integer_sound_component * const m_integer;
			View::push_button *m_dec;
			View::push_button *m_inc;
		};


		// Value Integer Factory


		class value_integer_gui_component_factory : public abstract_gui_component_factory {

		public:
			value_integer_gui_component_factory();

			std::unique_ptr<gui_sound_component> create_complete_component(
				const int x, const int y, Sound::data_source& source, const unsigned int channel_count) override;
			std::unique_ptr<gui_sound_component> create_complete_component(
				const int x, const int y, const Sound::abstract_form_answer& answer_form, const unsigned int channel_count) override;
		};

		//  Gain Integer Factory

		class gain_integer_gui_component_factory : public abstract_gui_component_factory {

		public:
			gain_integer_gui_component_factory();

			std::unique_ptr<gui_sound_component> create_complete_component(
				const int x, const int y, Sound::data_source& source, const unsigned int channel_count) override;
			std::unique_ptr<gui_sound_component> create_complete_component(
				const int x, const int y, const Sound::abstract_form_answer& answer_form, const unsigned int channel_count) override;
		};
	} /* Gui */

} /* Gammou */


#endif