

#include <iostream>
#include <cmath>
#include <cstring>

#include "vst3_plugin.h"

namespace Gammou {

	namespace VST3 {

		//	Plugin class implementation

		Plugin::Plugin()
		:	m_synthesizer_mutex(),
			m_master_circuit_processor(),
			m_polyphonic_circuit_processor(),
			m_synthesizer(
				m_master_circuit_processor, 
				m_polyphonic_circuit_processor, 
				2, 2, GAMMOU_SYNTHESIZER_CHANNEL_COUNT,
				GAMMOU_PARAMETER_INPUT_COUNT),
			m_gui(&m_synthesizer, &m_synthesizer_mutex),
			m_display(m_gui)
		{
			DEBUG_PRINT("Gammou Vst3 Plugin CTOR\n");
		}

		Plugin::~Plugin()
		{
			DEBUG_PRINT("Gammou Plugin DTOR\n");
		}

		inline void Plugin::lock_synthesizer()
		{
			m_synthesizer_mutex.lock();
			//DEBUG_PRINT("Plugin Lock\n");
		}

		inline void Plugin::unlock_synthesizer()
		{
			m_synthesizer_mutex.unlock();
			//DEBUG_PRINT("Plugin UNLock\n");
		}

		Steinberg::tresult PLUGIN_API Plugin::initialize(FUnknown * context)
		{
			const Steinberg::tresult result = SingleComponentEffect::initialize(context);

			if (result != Steinberg::kResultOk)
				return result;

			// Create Audio Input
			addAudioInput(USTRING("Stereo Input"), Steinberg::Vst::SpeakerArr::kStereo);

			// Create Audio Output
			addAudioOutput(USTRING("Stereo Output"), Steinberg::Vst::SpeakerArr::kStereo);

			//	Create Midi Input
			addEventInput(USTRING("Midi In"), 1); // 1 channel

			// Create Parameter inputs
			for (unsigned int i = 0; i < GAMMOU_PARAMETER_INPUT_COUNT; ++i) {
				std::string param_name = ("Parameter " + std::to_string(i));
				parameters.addParameter(new Steinberg::Vst::Parameter(USTRING(param_name.c_str()), i));
			}

			DEBUG_PRINT("Gammou Initialize\n");

			return Steinberg::kResultOk;
		}

		Steinberg::tresult PLUGIN_API Plugin::terminate()
		{
			return SingleComponentEffect::terminate();
		}

		Steinberg::tresult PLUGIN_API Plugin::setActive(Steinberg::TBool state)
		{
			// Todo
			DEBUG_PRINT("SET %s\n", state ? "Active" : "Inactive");
			return Steinberg::kResultOk;
		}

		Steinberg::tresult PLUGIN_API Plugin::canProcessSampleSize(Steinberg::int32 symbolicSampleSize)
		{
			return (symbolicSampleSize == Steinberg::Vst::kSample64 ||
				symbolicSampleSize == Steinberg::Vst::kSample32) ?
				Steinberg::kResultTrue : Steinberg::kResultFalse;
		}

		Steinberg::tresult PLUGIN_API Plugin::setupProcessing(Steinberg::Vst::ProcessSetup & newSetup)
		{
			DEBUG_PRINT("Gammou Setup processing : sample size = %u bits, sample rate = %lf\n",
				newSetup.symbolicSampleSize == Steinberg::Vst::kSample32 ? 32 : 64,
				newSetup.sampleRate);

			lock_synthesizer();
			m_synthesizer.set_sample_rate(newSetup.sampleRate);
			unlock_synthesizer();

			return SingleComponentEffect::setupProcessing(newSetup);
		}

		Steinberg::tresult PLUGIN_API Plugin::process(Steinberg::Vst::ProcessData & data)
		{
			lock_synthesizer();

			// Input parameter change (Parameter input)
			Steinberg::Vst::IParameterChanges *const param_changes = data.inputParameterChanges;

			if (param_changes != nullptr) {
				const unsigned int changed_param_count = param_changes->getParameterCount();
				
				for (unsigned int i = 0; i < changed_param_count; ++i) {
					Steinberg::Vst::IParamValueQueue *const param_data = param_changes->getParameterData(i);

					if (param_data != nullptr) {
						const unsigned int data_count = param_data->getPointCount();
						const unsigned int param_id = param_data->getParameterId();
						Steinberg::int32 sample_offset;
						double value;
						// For le moment, only the last point
						
						DEBUG_PRINT("Parameter data : id = %u, %u values : \n", param_id, data_count);

						for (unsigned int i = 0; i < data_count; ++i) {
							double v;
							Steinberg::int32 o;
							param_data->getPoint(i, o, v);
							DEBUG_PRINT("-> value[%u] : v = %lf ; o = %d\n", i, v, o);
						}


						if (Steinberg::kResultTrue == 
							param_data->getPoint(data_count - 1, sample_offset, value))
							m_synthesizer.set_parameter_value(value, param_id);
					}
				}
			}

			// Process Midi Input Events
			Steinberg::Vst::IEventList *event_list = data.inputEvents;

			if (event_list != nullptr) {
				const unsigned int event_count = event_list->getEventCount();

				for (unsigned int i = 0; i < event_count; ++i) {
					Steinberg::Vst::Event event;

					if (Steinberg::kResultOk == event_list->getEvent(i, event)) {
						
						//	TODO
						//	event.sampleOffset

						switch (event.type) {

						case Steinberg::Vst::Event::kNoteOnEvent:
							DEBUG_PRINT("Note On = %d, v = %f\n", event.noteOn.noteId, event.noteOn.velocity);
							if(event.noteOn.velocity < 0.01)
								m_synthesizer.send_note_off(event.noteOn.pitch, 0.5);
							else
								m_synthesizer.send_note_on(event.noteOn.pitch, event.noteOn.velocity);
							break;

						case Steinberg::Vst::Event::kNoteOffEvent:
							DEBUG_PRINT("Note Off = %d, v = %f\n", event.noteOff.noteId, event.noteOff.velocity);
							m_synthesizer.send_note_off(event.noteOff.pitch, event.noteOff.velocity);
							break;

						}
					}
				}
			}

			//	Process Audio

			//	No Silent flag (Todo check input silent flag)
			data.outputs[0].silenceFlags = 0;

			const unsigned int nb_samples = data.numSamples;
			//DEBUG_PRINT("Process %u sample\n", nb_samples);

			double input[2];
			double output[2];
				
			if (processSetup.symbolicSampleSize == Steinberg::Vst::kSample32) { // 32 bits
				float *output_buffer_left = data.outputs[0].channelBuffers32[0];
				float *output_buffer_right = data.outputs[0].channelBuffers32[1];

				float *input_buffer_left = data.inputs[0].channelBuffers32[0];
				float *input_buffer_right = data.inputs[0].channelBuffers32[1];

				for (unsigned int i = 0; i < nb_samples; 
					++i, ++output_buffer_left, ++output_buffer_right, ++input_buffer_left, ++input_buffer_right) {

					input[0] = static_cast<double>(*input_buffer_left);
					input[1] = static_cast<double>(*input_buffer_right);

					m_synthesizer.process(input, output);
					
					*output_buffer_left = static_cast<float>(output[0]);
					*output_buffer_right = static_cast<float>(output[1]);
				}

			}
			else { // 64 bit
				double *output_buffer_left = data.outputs[0].channelBuffers64[0];
				double *output_buffer_right = data.outputs[0].channelBuffers64[1];

				double *input_buffer_left = data.inputs[0].channelBuffers64[0];
				double *input_buffer_right = data.inputs[0].channelBuffers64[1];

				for (unsigned int i = 0; i < nb_samples; 
					++i, ++output_buffer_left, ++output_buffer_right, ++input_buffer_left, ++input_buffer_right) {

					input[0] = *input_buffer_left;
					input[1] = *input_buffer_right;

					m_synthesizer.process(input, output);

					*output_buffer_left = output[0];
					*output_buffer_right = output[1];
				}
			}

			unlock_synthesizer();

			return Steinberg::kResultOk;
		}

		Steinberg::tresult PLUGIN_API Plugin::setBusArrangements(Steinberg::Vst::SpeakerArrangement* inputs, Steinberg::int32 numIns,
			Steinberg::Vst::SpeakerArrangement* outputs, Steinberg::int32 numOuts)
		{
			// Host ask to change
			// TODO check coherence with synthesizer
			// And peut etre permetre au synth� de changer la ocnfiguration ?
			if (numIns != 1 ||
				numOuts != 1 ||
				outputs[0] != Steinberg::Vst::SpeakerArr::kStereo ||
				inputs[0] != Steinberg::Vst::SpeakerArr::kStereo)
				return Steinberg::kResultFalse;
			else
				return Steinberg::kResultOk;
		}

		// TODO ne pas, oublier get_editor state (????)

		Steinberg::tresult PLUGIN_API Plugin::setState(Steinberg::IBStream * state)
		{
			// Load State from data

			if (state != nullptr) {
				vst3_data_source data(state);

				try {

					if (m_gui.load_state(data)) {

						//	Inform host of new parameters value
						for (unsigned int i = 0; i < GAMMOU_PARAMETER_INPUT_COUNT; ++i) {
							const double param_value =
								m_synthesizer.get_parameter_value(i);
							// To the host
							setParamNormalized(i, param_value);
						}
						return Steinberg::kResultOk;
					}
				}
				catch (const std::exception& e) {
					DEBUG_PRINT("Load State Exception : '%s'\n", e.what());
				}
			}

			return Steinberg::kResultFalse;
		}

		Steinberg::tresult PLUGIN_API Plugin::getState(Steinberg::IBStream * state)
		{
			// Save State To data

			if (state != nullptr) {
				vst3_data_sink data(state);

				try {
					if (m_gui.save_state(data))
						return Steinberg::kResultOk;
				}
				catch (const std::exception& e) {
					DEBUG_PRINT("Save State Exception : '%s'\n", e.what());
				}
			}

			return Steinberg::kResultFalse;
		}


		Steinberg::IPlugView *Plugin::createView(const char* name)
		{
			return m_display.create_vst3_view_instance();
		}

		/*
			Persistence management
		*/

		static Steinberg::IBStream::IStreamSeekMode gammou_to_steinberg_seek_mode(
			Sound::abstract_data_stream::seek_mode mode)
		{

			switch (mode)
			{
			case Sound::abstract_data_stream::seek_mode::SET:
				return  Steinberg::IBStream::kIBSeekSet;
				break;

			case Sound::abstract_data_stream::seek_mode::END:
				return Steinberg::IBStream::kIBSeekEnd;
				break;

			//case Sound::data_stream::seek_mode::CURRENT:
			default:
				return Steinberg::IBStream::kIBSeekCur;
				break;
			}
		}

		vst3_data_source::vst3_data_source(Steinberg::IBStream * stream)
			: m_stream(stream)
		{
		}

		vst3_data_source::~vst3_data_source()
		{
		}

		bool vst3_data_source::seek(const int offset, Sound::abstract_data_stream::seek_mode mode)
		{
			const Steinberg::IBStream::IStreamSeekMode steinberg_mode = gammou_to_steinberg_seek_mode(mode);
			return (m_stream->seek(offset, steinberg_mode) == Steinberg::kResultOk);
		}

		unsigned int vst3_data_source::tell()
		{
			Steinberg::int64 pos;
			if (m_stream->tell(&pos) == Steinberg::kResultOk)
				return static_cast<unsigned int>(pos);
			else
				return 0;
		}

		unsigned int vst3_data_source::read(void * data, const unsigned int size)
		{
			Steinberg::int32 nb_read;
			if (m_stream->read(data, size, &nb_read) == Steinberg::kResultOk)
				return static_cast<unsigned int>(nb_read);
			else
				return 0u;
		}


		vst3_data_sink::vst3_data_sink(Steinberg::IBStream * stream)
			: m_stream(stream)
		{
		}

		vst3_data_sink::~vst3_data_sink()
		{
		}

		bool vst3_data_sink::seek(const int offset, Sound::abstract_data_stream::seek_mode mode)
		{
			const Steinberg::IBStream::IStreamSeekMode steinberg_mode = gammou_to_steinberg_seek_mode(mode);
			return (m_stream->seek(offset, steinberg_mode) == Steinberg::kResultOk);
		}

		unsigned int vst3_data_sink::tell()
		{
			Steinberg::int64 pos;
			if (m_stream->tell(&pos) == Steinberg::kResultOk)
				return static_cast<unsigned int>(pos);
			else
				return 0;
		}

		unsigned int vst3_data_sink::write(void * data, const unsigned int size)
		{
			Steinberg::int32 nb_written;
			if (m_stream->write(data, size, &nb_written) == Steinberg::kResultOk)
				return static_cast<unsigned int>(nb_written);
			else
				return 0;
		}

} /*  Vst */


} /* Gammou */
