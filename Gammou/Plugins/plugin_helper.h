#ifndef GAMMOU_PLUGIN_HELPER_H_
#define GAMMOU_PLUGIN_HELPER_H_

#ifdef _WIN32
#define DYNAMIC_EXPORT(decl) extern "C" { __declspec(dllexport) decl }
#else
#define DYNAMIC_EXPORT(decl) extern "C" { decl }
#endif

#include <synthesizer.h>
#include <plugin_management/abstract_plugin_factory.h>

#define EXPORT_DEFAULT_FACTORY(sound_component_type, factory_name, category, factory_id) \
	DYNAMIC_EXPORT( \
		abstract_plugin_factory *make_factory() \
			{ return new default_plugin_factory<sound_component_type>(factory_name, category, factory_id);}) \
	DYNAMIC_EXPORT( \
		void delete_factory(abstract_plugin_factory *factory) {delete factory;})
		
#endif
