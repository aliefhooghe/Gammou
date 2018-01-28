
#ifdef _WIN32
#include <Windows.h>
#define DYNAMIC_LIB_OPEN(path) (void*)LoadLibrary(path)
#define DYNAMIC_LIB_CLOSE(lib) FreeLibrary((HMODULE)lib)
#define DYNAMIC_LIB_SYMB(lib, name) GetProcAddress((HMODULE)lib, name)
#else
#include <dlfcn.h>
#define DYNAMIC_LIB_OPEN(path)  dlopen(path, RTLD_LAZY)
#define DYNAMIC_LIB_CLOSE(lib)	dlclose(lib)
#define DYNAMIC_LIB_SYMB(lib, name) dlsym(lib, name)
#endif

#include "main_factory.h"


namespace Gammou {

	namespace Sound {

		main_factory::main_factory()
		{
		}

		main_factory::~main_factory()
		{
			for (auto it = m_plugin_factory.begin(); it != m_plugin_factory.end(); ++it) {
				plugin_lib lib = it->second.first;
				abstract_plugin_factory *factory = it->second.second;

				if (lib.lib_handle == nullptr) { // registered builtin factory
					delete factory;
				}
				else { // dynamicaly loaded facotry 
					lib.factory_delete(factory);
					DYNAMIC_LIB_CLOSE(lib.lib_handle);
				}
				
			}
		}
		
		unsigned int main_factory::load_factory(const std::string & file_path)
		{
			plugin_lib lib = { nullptr, nullptr };

			lib.lib_handle = DYNAMIC_LIB_OPEN(file_path.c_str());
			if (lib.lib_handle == nullptr)
				throw std::runtime_error("Cannot open module file");

			factory_make_fct make_factory = nullptr;
			make_factory = reinterpret_cast<factory_make_fct>(DYNAMIC_LIB_SYMB(lib.lib_handle, GAMMOU_MAKE_FACTORY_SYMBOL));
			if (make_factory == nullptr) {
				DYNAMIC_LIB_CLOSE(lib.lib_handle);
				throw std::runtime_error("Cannot find " GAMMOU_MAKE_FACTORY_SYMBOL);
			}

			lib.factory_delete = reinterpret_cast<factory_delete_fct>(DYNAMIC_LIB_SYMB(lib.lib_handle, GAMMOU_DELETE_FACTORY_SYMBOL));
			if (lib.factory_delete == nullptr) {
				DYNAMIC_LIB_CLOSE(lib.lib_handle);
				throw std::runtime_error("Cannot find " GAMMOU_DELETE_FACTORY_SYMBOL);
			}

			abstract_plugin_factory *factory = make_factory();
			const unsigned int factory_id = factory->get_factory_id();
			m_plugin_factory[factory_id] = std::make_pair(lib, factory);
			
			return factory_id;
		}

		void main_factory::register_factory(abstract_plugin_factory * factory)
		{
			plugin_lib lib = { nullptr, nullptr };
			m_plugin_factory[factory->get_factory_id()] = std::make_pair(lib, factory);
		}

		const std::string & main_factory::get_factory_name(const unsigned int factory_id)
		{
			const abstract_plugin_factory *factory = factory_by_id(factory_id);
			return factory->get_name();
		}

		const std::string & main_factory::get_factory_description(const unsigned int factory_id)
		{
			const abstract_plugin_factory *factory = factory_by_id(factory_id);
			return factory->get_description();
		}

		const request_form_descriptor & main_factory::get_plugin_request_form(const unsigned int factory_id) const
		{
			const abstract_plugin_factory *factory = factory_by_id(factory_id);
			return factory->get_request_form();
		}

		abstract_sound_component * main_factory::get_new_sound_component(const unsigned int factory_id, data_source & data, const unsigned int channel_count)
		{
			abstract_plugin_factory *factory = factory_by_id(factory_id);
			return factory->get_new_sound_component(data, channel_count);
		}

		abstract_sound_component * main_factory::get_new_sound_component(const unsigned int factory_id, const answer_form_descriptor & answer_form, const unsigned int channel_count)
		{
			abstract_plugin_factory *factory = factory_by_id(factory_id);
			return factory->get_new_sound_component(answer_form, channel_count);
		}

		void main_factory::delete_sound_component(abstract_sound_component * component)
		{
			abstract_plugin_factory *factory = factory_by_id(component->get_factory_id());
			factory->delete_sound_component(component);
		}

		abstract_plugin_factory * main_factory::factory_by_id(const unsigned int factory_id) const
		{
			auto it = m_plugin_factory.find(factory_id);

			if (it == m_plugin_factory.end())
				throw std::domain_error("Factory id is not registered");
			else
				return it->second.second;
		}

	} /* Sound */

} /* Gammou */



