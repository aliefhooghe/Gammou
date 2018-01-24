

#include <iostream>
#include <cmath>

#include "vst3_plugin.h"

#include <Windows.h>

namespace Gammou {

	namespace Vst {

		//	Plugin class implementation

		Plugin::Plugin()
			: 
			m_synthesizer(0, 2, 128, 1),
			m_window(&m_synthesizer, 1000, 600)
		{
			DEBUG_PRINT("Gammou CTOR\n");
		}

		Plugin::~Plugin()
		{
			DEBUG_PRINT("Gammou DTOR\n");
		}

		Steinberg::tresult PLUGIN_API Plugin::initialize(FUnknown * context)
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

		Steinberg::tresult PLUGIN_API Plugin::terminate()
		{
			return SingleComponentEffect::terminate();
		}

		Steinberg::tresult PLUGIN_API Plugin::setActive(Steinberg::TBool state)
		{
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
			m_synthesizer.set_sample_rate(newSetup.sampleRate);
			return SingleComponentEffect::setupProcessing(newSetup);
		}

		Steinberg::tresult PLUGIN_API Plugin::process(Steinberg::Vst::ProcessData & data)
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
						//	event.sampleOffset

						switch (event.type) {

						case Steinberg::Vst::Event::kNoteOnEvent:
							m_synthesizer.send_note_on(event.noteOn.pitch, event.noteOn.velocity);
							break;

						case Steinberg::Vst::Event::kNoteOffEvent:
							m_synthesizer.send_note_off(event.noteOff.pitch, event.noteOff.velocity);
							break;

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
			double output[2] = { 0.0, 0.0 };

			if (processSetup.symbolicSampleSize == Steinberg::Vst::kSample32) {
				float *outputBufferLeft = data.outputs[0].channelBuffers32[0];
				float *outputBufferRight = data.outputs[0].channelBuffers32[1];

				for (unsigned int i = 0; i < nbSamples; ++i, ++outputBufferLeft, ++outputBufferRight) {
					//m_synthesizer.process(nullptr, output);

					*outputBufferLeft = 0.0;
					*outputBufferRight = 0.0;
				}
			}
			else { // 64 bit
				double *outputBufferLeft = data.outputs[0].channelBuffers64[0];
				double *outputBufferRight = data.outputs[0].channelBuffers64[1];

				for (unsigned int i = 0; i < nbSamples; ++i, ++outputBufferLeft, ++outputBufferRight) {
					//m_synthesizer.process(nullptr, output);

					*outputBufferLeft = output[0];
					*outputBufferRight = output[1];
				}
			}

			return Steinberg::kResultOk;
		}

		Steinberg::tresult PLUGIN_API Plugin::setBusArrangements(Steinberg::Vst::SpeakerArrangement* inputs, Steinberg::int32 numIns,
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


		Steinberg::IPlugView *Plugin::createView(const char* name)
		{
			return m_window.create_vst3_view_instance();
		}



	} /*  Vst */


} /* Gammou */

