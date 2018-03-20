
#include "../gui_sound_component.h"
#include "../abstract_gui_component_factory.h"

namespace Gammou {

	namespace Gui {

		class knob_sound_component : public Sound::sound_component {

		public:
			knob_sound_component(const unsigned int channel_count, 
				const double initial_normalized_value = 0.2);
			~knob_sound_component() {}

			void initialize_process() override;
			void process(const double input[]) override;
			unsigned int save_state(Sound::data_sink& data) override;
			
			void set_normalized_value(const double normalized_value);
			double get_normalized_value() const;

		private: 
			Sound::multi_channel_variable<double> m_prev_out; // bof bof
			double m_normalized_value;
			double m_output_value;;
		};


		class knob_gui_component : public gui_sound_component {

		public:
			knob_gui_component(knob_sound_component *knob, const unsigned int x, const unsigned int y);
			~knob_gui_component() {}

		private:
			knob_sound_component *const m_knob_sound_component;
		};

		class knob_complete_component_factory : public complete_component_factory {

		public:
			knob_complete_component_factory();

			complete_component create_complete_component(const int x, const int y, Sound::data_source& source, const unsigned int channel_count) override;
			complete_component create_complete_component(const int x, const int y, const Sound::abstract_form_answer& answer_form, const unsigned int channel_count) override;
		};
	}




} /* Gammou */