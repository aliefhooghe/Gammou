#ifndef SLIDER_GUI_COMPONENT_H_
#define SLIDER_GUI_COMPONENT_H_

#include "gui_sound_component.h"
#include "abstract_gui_component_factory.h"
#include "control_sound_component.h"
#include "gain_sound_component.h"
#include "value_sound_component.h"
#include "control_component_ids.h"

namespace Gammou {

	namespace Gui {

		class slider_gui_component : public gui_sound_component {

		public:
			slider_gui_component(
				control_sound_component *control, 
				const unsigned int x, 
                const unsigned int y,
                const unsigned int width = 250);
			~slider_gui_component() {}

		private:
			control_sound_component * const m_control;
		};


		//	Value Slider Factory

		class value_slider_gui_component_factory : public abstract_gui_component_factory {

		public:
			value_slider_gui_component_factory();

			std::unique_ptr<gui_sound_component> create_complete_component(
				const int x, const int y, Sound::data_input_stream& source, const unsigned int channel_count) override;
			std::unique_ptr<gui_sound_component> create_complete_component(
				const int x, const int y, const Sound::answer_form& answer_form, const unsigned int channel_count) override;
		};


		//	Gain Slider Factory

		class gain_slider_gui_component_factory : public abstract_gui_component_factory {

		public:
			gain_slider_gui_component_factory();

			std::unique_ptr<gui_sound_component> create_complete_component(
				const int x, const int y, Sound::data_input_stream& source, const unsigned int channel_count) override;
			std::unique_ptr<gui_sound_component> create_complete_component(
				const int x, const int y, const Sound::answer_form& answer_form, const unsigned int channel_count) override;
		};

	}

}

#endif