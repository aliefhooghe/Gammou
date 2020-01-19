#ifndef GAMMOU_KNOB_GUI_COMPONENT_H_
#define GAMMOU_KNOB_GUI_COMPONENT_H_

#include "gui_sound_component.h"
#include "abstract_gui_component_factory.h"
#include "control_sound_component.h"
#include "gain_sound_component.h"
#include "value_sound_component.h"

#include "midi_driver/midi_driver.h"
#include "control_component_ids.h"

namespace Gammou {

	namespace Gui {

		class knob_gui_component : public gui_sound_component {

		public:
			knob_gui_component(
				Sound::midi_driver& driver,
				control_sound_component *control,
				const unsigned int x, const unsigned int y);
			~knob_gui_component() override;

		private:
			control_sound_component * const m_control;
			Sound::midi_driver& m_midi_driver;
		};


		//	Value Knob Factory

		class value_knob_gui_component_factory : public abstract_gui_component_factory {

		public:
			value_knob_gui_component_factory(Sound::midi_driver& driver);

			std::unique_ptr<gui_sound_component> create_gui_component(
				const int x, const int y, Sound::data_input_stream& source, const unsigned int channel_count) override;
			std::unique_ptr<gui_sound_component> create_gui_component(
				const int x, const int y, const Sound::answer_form& answer_form, const unsigned int channel_count) override;

		private:
			Sound::midi_driver& m_midi_driver;
		};


		//	Gain Knob Factory

		class gain_knob_gui_component_factory : public abstract_gui_component_factory {

		public:
			gain_knob_gui_component_factory(Sound::midi_driver& driver);

			std::unique_ptr<gui_sound_component> create_gui_component(
				const int x, const int y, Sound::data_input_stream& source, const unsigned int channel_count) override;
			std::unique_ptr<gui_sound_component> create_gui_component(
				const int x, const int y, const Sound::answer_form& answer_form, const unsigned int channel_count) override;

		private:
			Sound::midi_driver& m_midi_driver;
		};

	}

}

#endif