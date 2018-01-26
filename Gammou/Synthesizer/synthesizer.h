#ifndef SYNTHESIZER_H_
#define SYNTHESIZER_H_


#include "sound_component/sound_component.h"
#include "synthesizer_circuits/master_circuit.h"
#include "synthesizer_circuits/polyphonic_circuit.h"
#include "sound_component/standard_sound_component.h"


namespace Gammou {

	namespace Sound {

		class synthesizer{

			/*
			*			pour chaque cannal :
			*				-> lifetime : uint
			*				-> Midi note : uchar
			*				->
			*/

		enum { INVALID_CHANNEL = 0xFFFFFF , INFINITE_LIFETIME = 0xFFFFFFFF, NO_NOTE = 0xFF};

		public:
			synthesizer(const unsigned int main_input_count,
						const unsigned int main_output_count,
						const unsigned int channel_count,
						const unsigned int automation_count,
						const unsigned int master_to_polyphonic_count = 2,
						const unsigned int polyphonic_to_master_count = 2,
						const unsigned int sample_rate = 44100,
						const float zero_lifetime = 1.0);

			~synthesizer();

			void process(const double input[], double output[]);

			void send_note_on(const char midi_note, const double velocity);
			void send_note_off(const char midi_note, const double velocity);

			void add_sound_component_on_master_circuit(abstract_sound_component *component);
			void add_sound_component_on_polyphonic_circuit(polyphonic_sound_component *component);

			void set_sample_rate(const double sample_rate);

			// polyphonic_circuit_ component

			Process::abstract_component<double> *get_polyphonic_circuit_gpar_input();
			Process::abstract_component<double> *get_polyphonic_circuit_master_input();
			Process::abstract_component<double> *get_polyphonic_circuit_automation_input();
			Process::abstract_component<double> *get_polyphonic_circuit_output();

			// master_circuit_ component

			Process::abstract_component<double> *get_master_main_input();
			Process::abstract_component<double> *get_master_main_output();

			Process::abstract_component<double> *get_master_circuit_polyphonic_input();
			Process::abstract_component<double> *get_master_circuit_polyphonic_output();
			Process::abstract_component<double> *get_master_circuit_automation_input();

			//
			unsigned int get_automation_input_count() const;
			void set_automation_value(const double value, const unsigned int automation_id);

		private:
			unsigned int get_new_channel();
			void free_channel(const std::vector<unsigned int>::iterator& it);

			master_circuit m_master_circuit;
			polyphonic_circuit m_polyphonic_circuit;
			

			// Channels allocation
			std::vector<unsigned int> m_channels;
			std::vector<unsigned int>::iterator m_running_channels_end;

			//	Channels lifetime
			std::vector<unsigned int> m_channels_lifetime;
			unsigned int m_channel_zero_lifetime; // No const (set_sample rate)

			// Channel note
			std::vector<unsigned char> m_channels_midi_note;

			static const double m_note_frequencies[127];
		};


	} /* Sound */

} /* Gammou */




#endif /* SYNTHESIZER_H_ */
