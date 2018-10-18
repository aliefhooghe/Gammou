#ifndef SYNTHESIZER_H_
#define SYNTHESIZER_H_


#include "sound_component/sound_component.h"
#include "synthesizer_circuits/master_circuit.h"
#include "synthesizer_circuits/polyphonic_circuit.h"
#include "sound_processing_helper.h"

#include "builtin_components.h"


namespace Gammou {

	namespace Sound {

		class synthesizer{

		enum { INVALID_CHANNEL = 0xFFFFFF , INFINITE_LIFETIME = 0xFFFFFFFF, NO_NOTE = 0xFF};

		public:

			enum class keyboard_mode { POLYPHONIC, LEGATO };

			synthesizer(
				Process::abstract_frame_processor<double>& master_circuit_processor,
				Process::abstract_frame_processor<double>& polyphonic_circuit_processor,
				const unsigned int main_input_count,
				const unsigned int main_output_count,
				const unsigned int channel_count,
				const unsigned int parameter_count,
				const unsigned int master_to_polyphonic_count = 2,
				const unsigned int polyphonic_to_master_count = 2,
				const unsigned int sample_rate = 44100,
                const float zero_lifetime = 0.001f);

			~synthesizer();

			void process(const double input[], double output[]);
			void set_master_volume(const double volume_order); // smoothing is applied by master circuit

			// Midi note : 
			void send_note_on(const unsigned int midi_note, const double velocity);
			void send_note_off(const unsigned int midi_note, const double velocity);

            //  Keyboard Mode
            void set_keyboard_mode(const keyboard_mode mode);
            keyboard_mode get_keyboard_mode() const;

			void add_sound_component_on_master_circuit(abstract_sound_component *component);
			void add_sound_component_on_polyphonic_circuit(abstract_sound_component *component);

			void set_sample_rate(const double sample_rate);

			// polyphonic_circuit_ component

			Process::abstract_component<double> *get_polyphonic_circuit_midi_input();
			Process::abstract_component<double> *get_polyphonic_circuit_master_output();
			Process::abstract_component<double> *get_polyphonic_circuit_parameter_input();
			Process::abstract_component<double> *get_polyphonic_circuit_master_input();

			// master_circuit_ component

			Process::abstract_component<double> *get_master_main_input();
			Process::abstract_component<double> *get_master_main_output();

			Process::abstract_component<double> *get_master_circuit_polyphonic_input();
			Process::abstract_component<double> *get_master_circuit_polyphonic_output();
			Process::abstract_component<double> *get_master_circuit_parameter_input();

			//
			unsigned int get_parameter_input_count() const;
			void set_parameter_value(const double value, const unsigned int automation_id);
            double get_parameter_value(const unsigned int automation_id) const;

			unsigned int get_channel_count() const;

            //

		private:
			unsigned int get_new_channel();
            unsigned int get_running_channel_count();
			void free_channel(const std::vector<unsigned int>::iterator& it);

			// Circuits
			master_circuit m_master_circuit;
			polyphonic_circuit m_polyphonic_circuit;
			
			// Channels allocation
			std::vector<unsigned int> m_channels;
			std::vector<unsigned int>::iterator m_running_channels_end;

			//	Channels lifetime
			std::vector<unsigned int> m_channels_lifetime;
			const double m_channel_zero_lifetime;
			unsigned int m_channel_zero_sample_count; // No const (set_sample rate)

			// Channel note
			std::vector<unsigned int> m_channels_midi_note;

            //  Keyboard mode
            keyboard_mode m_keyboard_mode;

			static const double m_note_frequencies[128];
		};


	} /* Sound */

} /* Gammou */




#endif /* SYNTHESIZER_H_ */
