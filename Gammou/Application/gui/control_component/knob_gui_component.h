#ifndef GAMMOU_KNOB_GUI_COMPONENT_H_
#define GAMMOU_KNOB_GUI_COMPONENT_H_

#include "gui_sound_component.h"
#include "abstract_gui_component_factory.h"
#include "control_sound_component.h"
#include "gain_sound_component.h"
#include "value_sound_component.h"
#include "control_component_ids.h"

namespace Gammou {

	namespace Gui {

		class knob_gui_component : public gui_sound_component {

		public:
			knob_gui_component(control_sound_component *control, const unsigned int x, const unsigned int y);
			~knob_gui_component() {}

		private:
			control_sound_component * const m_control;
		};


		//	Value Knob Factory

		class value_knob_gui_component_factory : public abstract_gui_component_factory {

		public:
			value_knob_gui_component_factory();

			complete_component create_complete_component(
				const int x, const int y, Sound::data_source& source, const unsigned int channel_count) override;
			complete_component create_complete_component(
				const int x, const int y, const Sound::abstract_form_answer& answer_form, const unsigned int channel_count) override;
		};


		//	Gain Knob Factory

		class gain_knob_gui_component_factory : public abstract_gui_component_factory {

		public:
			gain_knob_gui_component_factory();

			complete_component create_complete_component(
				const int x, const int y, Sound::data_source& source, const unsigned int channel_count) override;
			complete_component create_complete_component(
				const int x, const int y, const Sound::abstract_form_answer& answer_form, const unsigned int channel_count) override;
		};

	}

}

#endif