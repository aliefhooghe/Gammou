
#include "public.sdk/source/main/pluginfactoryvst3.h"
#include "pluginterfaces/vst/ivstcomponent.h"
#include "pluginterfaces/vst/ivstaudioprocessor.h"


#include "vst3_plugin.h"

#define plugin_name_string "Gammou"

Steinberg::FUnknown *createGammouInstance(void *context)
{
	return (Steinberg::Vst::IAudioProcessor*)(new Gammou::Vst::Plugin());
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
		INLINE_UID(42, 43, 44, 45),
		PClassInfo::kManyInstances,
		kVstAudioEffectClass,
		plugin_name_string,
		0,			// Non distributable
		Steinberg::Vst::PlugType::kFxInstrument,
		"0.0.0.0",
		kVstVersionString,
		createGammouInstance)

END_FACTORY