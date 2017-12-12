

#include <iostream>
#include <cmath>

#include "GammouPlugin.h"


#include <Windows.h>

namespace Gammou {

	namespace Vst {

		//	GammouPlugin class implementation

		GammouPlugin::GammouPlugin()
		{
			m_t = 0.0;
			m_freq = 111.0;
			DEBUG_PRINT("Gammou CTOR\n");
			m_window = new GammouPluginWindow();
		}

		GammouPlugin::~GammouPlugin()
		{
			delete m_window;
		}

		Steinberg::tresult PLUGIN_API GammouPlugin::initialize(FUnknown * context)
		{
			Steinberg::tresult result = SingleComponentEffect::initialize(context);

			if (result != Steinberg::kResultOk)
				return result;

			// Create Audio Output
			addAudioOutput(USTRING("Stereo Out"), Steinberg::Vst::SpeakerArr::kStereo);

			//	Create Midi Input
			addEventInput(USTRING("Midi In"), 1); // 1 channel

			DEBUG_PRINT("Gammou Initialize\n");

			return Steinberg::kResultOk;
		}

		Steinberg::tresult PLUGIN_API GammouPlugin::terminate()
		{
			return SingleComponentEffect::terminate();
		}

		Steinberg::tresult PLUGIN_API GammouPlugin::setActive(Steinberg::TBool state)
		{
			return Steinberg::kResultOk;
		}

		Steinberg::tresult PLUGIN_API GammouPlugin::canProcessSampleSize(Steinberg::int32 symbolicSampleSize)
		{
			return (symbolicSampleSize == Steinberg::Vst::kSample64 ||
				symbolicSampleSize == Steinberg::Vst::kSample32) ?
				Steinberg::kResultTrue : Steinberg::kResultFalse;
		}

		Steinberg::tresult PLUGIN_API GammouPlugin::setupProcessing(Steinberg::Vst::ProcessSetup & newSetup)
		{
			//	TODO : peut etre Prendre en compte newSetup.sampleRate et newSetup.maxSamplesPerBlock
			DEBUG_PRINT("Gammou Setup processing : sample size = %u bits, sample rate = %lf\n",
				newSetup.symbolicSampleSize == Steinberg::Vst::kSample32 ? 32 : 64,
				newSetup.sampleRate);
			return SingleComponentEffect::setupProcessing(newSetup);
		}

		Steinberg::tresult PLUGIN_API GammouPlugin::process(Steinberg::Vst::ProcessData & data)
		{
			// Input parameter change : Nothing for le moment

			// Input Events
			Steinberg::Vst::IEventList *eventList = data.inputEvents;

			if (eventList != nullptr) {
				unsigned int eventCount = eventList->getEventCount();
				for (unsigned int i = 0; i < eventCount; ++i) {
					Steinberg::Vst::Event event;
					if (Steinberg::kResultOk == eventList->getEvent(i, event)) {
						//	TODO
						//	event.type == kNoteOnEvent ?, etc...
						//	event.sampleOffset

						if (event.type == Steinberg::Vst::Event::kNoteOnEvent) {
							m_freq += 111.0;
						}
					}
				}
			}

			//	Process Audio output
			if (data.numOutputs == 0)
				return Steinberg::kResultOk;

			//	No Silent flag 
			data.outputs[0].silenceFlags = 0;

			const unsigned int nbSamples = data.numSamples;
			const double dt = 1.0 / 44100.0;

			if (processSetup.symbolicSampleSize == Steinberg::Vst::kSample32) {
				float *outputBufferLeft = data.outputs[0].channelBuffers32[0];
				float *outputBufferRight = data.outputs[0].channelBuffers32[1];

				for (unsigned int i = 0; i < nbSamples; ++i, ++outputBufferLeft, ++outputBufferRight) {
					*outputBufferLeft = std::sin(6.28 * m_freq * m_t);
					*outputBufferRight = std::sin(6.28 * m_freq * m_t);
					m_t += dt;
				}
			}
			else { // 64 bit
				double *outputBufferLeft = data.outputs[0].channelBuffers64[0];
				double *outputBufferRight = data.outputs[0].channelBuffers64[1];

				for (unsigned int i = 0; i < nbSamples; ++i, ++outputBufferLeft, ++outputBufferRight) {
					*outputBufferLeft = std::sin(6.28 * m_freq * m_t);
					*outputBufferRight = std::sin(6.28 * m_freq * m_t);
					m_t += dt;
				}
			}

			return Steinberg::kResultOk;
		}

		Steinberg::tresult PLUGIN_API GammouPlugin::setBusArrangements(Steinberg::Vst::SpeakerArrangement* inputs, Steinberg::int32 numIns,
			Steinberg::Vst::SpeakerArrangement* outputs, Steinberg::int32 numOuts)
		{
			// Host ask to change
			if (numIns != 0 ||
				numOuts != 1 ||
				outputs[0] != Steinberg::Vst::SpeakerArr::kStereo)
				return Steinberg::kResultFalse;
			else
				return Steinberg::kResultOk;
		}


		Steinberg::IPlugView *GammouPlugin::createView(const char* name)
		{
			return m_window->create_vst3_view_instance();
		}


	
	} /*  Vst */


} /* Gammou */

