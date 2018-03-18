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

#define GAMMOU_VST_AUTOMATION_INPUT_COUNT 8

namespace Gammou {

	namespace Vst {

		class Plugin : public Steinberg::Vst::SingleComponentEffect {

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

			Steinberg::tresult PLUGIN_API setBusArrangements(Steinberg::Vst::SpeakerArrangement* inputs, Steinberg::int32 numIns,
				Steinberg::Vst::SpeakerArrangement* outputs, Steinberg::int32 numOuts) override;

			Steinberg::tresult PLUGIN_API setState(Steinberg::IBStream* state) override;
			Steinberg::tresult PLUGIN_API getState(Steinberg::IBStream* state) override;

			//	Editor override
			Steinberg::IPlugView *PLUGIN_API createView(const char* name) override;

		private:
			inline void lock_synthesizer();
			inline void unlock_synthesizer();

			std::mutex m_synthesizer_mutex;
			Sound::synthesizer m_synthesizer;
			Gui::synthesizer_gui m_window;
		};


		/*
			Persistence management
		*/

		class vst3_data_source : public Sound::data_source {

		public:
			vst3_data_source(Steinberg::IBStream *stream);
			vst3_data_source(vst3_data_source&) = delete;
			~vst3_data_source();

			bool seek(const int offset, Sound::data_stream::seek_mode mode) override;
			unsigned int tell() override;
			unsigned int read(void *data, const unsigned int size) override;
		
		private:
			Steinberg::IBStream *m_stream;
		};

		class vst3_data_sink : public Sound::data_sink {

		public:
			vst3_data_sink(Steinberg::IBStream *stream);
			vst3_data_sink(vst3_data_sink&) = delete;
			~vst3_data_sink();

			bool seek(const int offset, Sound::data_stream::seek_mode mode) override;
			unsigned int tell() override;
			unsigned int write(void *data, const unsigned int size) override;

		private:
			Steinberg::IBStream *m_stream;

		};

	} /* namespace Vst */

} /* namespace Gammou */



#endif