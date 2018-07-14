

//------
#include "public.sdk/source/main/pluginfactoryvst3.h"
#include "pluginterfaces/vst/ivstcomponent.h"
#include "pluginterfaces/vst/ivstaudioprocessor.h"

#include "vst3_plugin.h"

static const Steinberg::FUID gammouUID(42, 43, 44, 45);

#define plugin_name_string "Gammou"

Steinberg::FUnknown *createGammouInstance(void *context)
{
	DEBUG_PRINT("Create Gammou Instance\n");
	return (Steinberg::Vst::IAudioProcessor*)(new Gammou::VST3::Plugin());
}

bool InitModule()
{
	return true;
}

bool DeinitModule()
{
	return true;
}

BEGIN_FACTORY_DEF("Arthur Liefhooghe",
					"",
					"aliefhooghe@enseirb-matmeca.fr")

	DEF_CLASS2(
		INLINE_UID_FROM_FUID(gammouUID),
		PClassInfo::kManyInstances,
		kVstAudioEffectClass,
		plugin_name_string,
		0,			// Non distributable
		Steinberg::Vst::PlugType::kFxInstrument,
		"0.0.0.0",
		kVstVersionString,
		createGammouInstance)

END_FACTORY


