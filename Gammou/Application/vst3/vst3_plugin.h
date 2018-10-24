#ifndef GAMMOU_VST3_H_
#define GAMMOU_VST3_H_

/*
 *		Plugin.h
 */

#include <mutex>

#include "../../debug.h"

#include "public.sdk/source/vst/vstsinglecomponenteffect.h" // first !!!!

#include "pluginterfaces/base/ibstream.h"
#include "pluginterfaces/base/ftypes.h"
#include "pluginterfaces/vst/ivstevents.h"
#include "pluginterfaces/vst/ivstparameterchanges.h"
#include "pluginterfaces/base/ustring.h"

#include "../gui/synthesizer_gui.h"
#include "synthesizer.h"
#include "jit_frame_processor/jit_frame_processor.h"

#include "../audio_backend/abstract_audio_backend.h"

#include <view.h>

#define GAMMOU_VST3_INPUT_COUNT 2
#define GAMMOU_VST3_OUTPUT_COUNT 2
#define GAMMOU_VST3_CHANNEL_COUNT 128
#define GAMMOU_VST3_PARAMETER_COUNT 16

namespace Gammou {

	namespace VST3 {

		class Plugin : 
			public Steinberg::Vst::SingleComponentEffect,
			public AudioBackend::abstract_audio_backend {

		public:
			Plugin();
			~Plugin();

			//	AudioEffect override
			Steinberg::tresult PLUGIN_API initialize(FUnknown* context) override;
			Steinberg::tresult PLUGIN_API terminate() override;
			Steinberg::tresult PLUGIN_API setActive(Steinberg::TBool state) override;

			Steinberg::tresult PLUGIN_API canProcessSampleSize(Steinberg::int32 symbolicSampleSize) override;
			Steinberg::tresult PLUGIN_API setupProcessing(Steinberg::Vst::ProcessSetup& newSetup) override;
			Steinberg::tresult PLUGIN_API process(Steinberg::Vst::ProcessData& data) override;

			Steinberg::tresult PLUGIN_API setBusArrangements(
				Steinberg::Vst::SpeakerArrangement* inputs, Steinberg::int32 numIns,
				Steinberg::Vst::SpeakerArrangement* outputs, Steinberg::int32 numOuts) override;

			Steinberg::tresult PLUGIN_API setState(Steinberg::IBStream* state) override;
			Steinberg::tresult PLUGIN_API getState(Steinberg::IBStream* state) override;

			//	Editor override
			Steinberg::IPlugView *PLUGIN_API createView(const char* name) override;

			//	Audio Backend override
			double get_parameter_value(
				const unsigned int index) override;

			void set_parameter_value(
				const unsigned int index, const double value) override;

			unsigned int get_parameter_count() override;

		private:
			inline void lock_synthesizer();
			inline void unlock_synthesizer();

			std::mutex m_synthesizer_mutex;
			//Sound::jit_frame_processor m_master_circuit_processor;
			Process::bytecode_frame_processor<double> m_master_circuit_processor;
			Process::bytecode_frame_processor<double> m_polyphonic_circuit_processor;
			Sound::synthesizer m_synthesizer;
			
			Gui::synthesizer_gui m_gui;
			View::vst3_display m_display;
		};


		/*
			Persistence management
		*/

		class vst3_data_source : public Sound::data_input_stream {

		public:
			vst3_data_source(Steinberg::IBStream *stream);
			vst3_data_source(vst3_data_source&) = delete;
			~vst3_data_source();

			bool seek(const int offset, Sound::abstract_data_stream::seek_mode mode) override;
			unsigned int tell() override;
			unsigned int read(void *data, const unsigned int size) override;
		
		private:
			Steinberg::IBStream *m_stream;
		};

		class vst3_data_sink : public Sound::data_output_stream {

		public:
			vst3_data_sink(Steinberg::IBStream *stream);
			vst3_data_sink(vst3_data_sink&) = delete;
			~vst3_data_sink();

			bool seek(const int offset, Sound::abstract_data_stream::seek_mode mode) override;
			unsigned int tell() override;
			unsigned int write(void *data, const unsigned int size) override;

		private:
			Steinberg::IBStream *m_stream;

		};

	} /* namespace Vst */

} /* namespace Gammou */



#endif