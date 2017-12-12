
#include "public.sdk/source/main/pluginfactoryvst3.h"
#include "pluginterfaces/vst/ivstcomponent.h"
#include "pluginterfaces/vst/ivstaudioprocessor.h"


#include "plugin.h"

#define stringPluginName "Gammou"

Steinberg::FUnknown *createGammouInstance(void *context)
{
	return (Steinberg::Vst::IAudioProcessor*)(new Gammou::Vst::GammouPlugin());
}

bool InitModule()
{
	return true;
}

bool DeinitModule()
{
	return true;
}

BEGIN_FACTORY_DEF("vendor",
					"url",
					"email")

	DEF_CLASS2(
		INLINE_UID(42, 43, 44, 45),
		PClassInfo::kManyInstances,
		kVstAudioEffectClass,
		stringPluginName,
		0,			// Non distributable
		Steinberg::Vst::PlugType::kInstrumentSynth,
		"0.0.0.0",
		kVstVersionString,
		createGammouInstance)

END_FACTORY