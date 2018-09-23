
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
#ifdef _WIN32
            // Avoid error popup showing msg whenLoadLibrary fail
			SetErrorMode(SEM_FAILCRITICALERRORS);
#endif
		}

		main_factory::~main_factory()
		{
            //  Everything cleaned bu unique_ptr !
		}
		
		unsigned int main_factory::load_factory(const std::string & file_path)
		{
            void *lib_handle = DYNAMIC_LIB_OPEN(file_path.c_str());
            if (lib_handle == nullptr)
				throw std::runtime_error("Cannot open Gammou Plugin file");

			factory_make_fct make_factory = nullptr;
            make_factory =
                    reinterpret_cast<factory_make_fct>(DYNAMIC_LIB_SYMB(lib_handle, GAMMOU_MAKE_FACTORY_SYMBOL));
			if (make_factory == nullptr) {
                DYNAMIC_LIB_CLOSE(lib_handle);
				throw std::runtime_error("Cannot find " GAMMOU_MAKE_FACTORY_SYMBOL);
			}

            factory_delete_fct factory_delete =
                    reinterpret_cast<factory_delete_fct>(DYNAMIC_LIB_SYMB(lib_handle, GAMMOU_DELETE_FACTORY_SYMBOL));
            if (factory_delete == nullptr) {
                DYNAMIC_LIB_CLOSE(lib_handle);
				throw std::runtime_error("Cannot find " GAMMOU_DELETE_FACTORY_SYMBOL);
			}

			abstract_plugin_factory *factory = make_factory();
			const unsigned int factory_id = factory->get_factory_id();
            m_plugin_factory[factory_id] =
                    std::unique_ptr<abstract_plugin_factory, factory_deleter>(
                        factory,
                        get_plugin_factory_deleter(lib_handle, factory_delete));
			
			return factory_id;
		}

        void main_factory::add_factory(std::unique_ptr<abstract_plugin_factory> && factory)
		{
            const unsigned int factory_id = factory->get_factory_id();
            m_plugin_factory[factory_id] =
                std::unique_ptr<abstract_plugin_factory, factory_deleter>(
                    factory.release(), get_default_factory_deleter());
		}

		bool main_factory::check_factory_presence(const unsigned int factory_id) const
		{
			return (m_plugin_factory.find(factory_id) != m_plugin_factory.end());
		}

		const std::string & main_factory::get_factory_name(const unsigned int factory_id)
		{
			const abstract_plugin_factory *factory = factory_by_id(factory_id);
			return factory->get_name();
		}

		const std::string & main_factory::get_factory_description(const unsigned int factory_id)
		{
			const abstract_plugin_factory *factory = factory_by_id(factory_id);
			return factory->get_category();
		}

		const request_form & main_factory::get_plugin_request_form(const unsigned int factory_id)
		{
			abstract_plugin_factory *factory = factory_by_id(factory_id);
			return factory->get_request_form();
		}

		abstract_sound_component * main_factory::get_new_sound_component(
			const unsigned int factory_id, 
			data_input_stream & data, 
			const unsigned int channel_count)
		{
			abstract_plugin_factory *factory = factory_by_id(factory_id);
			return factory->get_new_sound_component(data, channel_count);
		}

		abstract_sound_component * main_factory::get_new_sound_component(
			const unsigned int factory_id, 
			const answer_form& answer_form, 
			const unsigned int channel_count)
		{
			abstract_plugin_factory *factory = factory_by_id(factory_id);
			return factory->get_new_sound_component(answer_form, channel_count);
		}
/*
		void main_factory::delete_sound_component(abstract_sound_component * component)
		{
			abstract_plugin_factory *factory = factory_by_id(component->get_factory_id());
			factory->delete_sound_component(component);
		}
*/
		abstract_plugin_factory * main_factory::factory_by_id(const unsigned int factory_id) const
		{
			auto it = m_plugin_factory.find(factory_id);

			if (it == m_plugin_factory.end())
				throw std::domain_error("Factory id is not registered");
			else
                return it->second.get();
		}

        main_factory::factory_deleter main_factory::get_default_factory_deleter()
        {
            return [](abstract_plugin_factory *factory)
            {
                DEBUG_PRINT("Deleting built-in factory : '%s'\n", factory->get_name().c_str());
                delete factory;
            };
        }

        main_factory::factory_deleter main_factory::get_plugin_factory_deleter(void *lib_handle, factory_delete_fct delete_fct)
        {
            return [lib_handle, delete_fct](abstract_plugin_factory *factory)
            {
                DEBUG_PRINT("Deleting plugin loaded factory : '%s'\n", factory->get_name().c_str());
                delete_fct(factory);
                DYNAMIC_LIB_CLOSE(lib_handle);
            };
        }

	} /* Sound */

} /* Gammou */



