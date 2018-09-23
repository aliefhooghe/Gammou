
#ifndef GAMMOU_MAIN_FACTORY_H_
#define GAMMOU_MAIN_FACTORY_H_

#include <map>
#include <functional>
#include "abstract_plugin_factory.h"



#define GAMMOU_MAKE_FACTORY_SYMBOL "make_factory"
#define GAMMOU_DELETE_FACTORY_SYMBOL "delete_factory"

namespace Gammou {

	namespace Sound {

		class main_factory {

			typedef void (*factory_delete_fct)(abstract_plugin_factory*);
			typedef abstract_plugin_factory *(*factory_make_fct)(void);
            typedef std::function<void(abstract_plugin_factory*)> factory_deleter;

		public:
			main_factory();
			~main_factory();

			//-------------
			unsigned int load_factory(const std::string& file_path);
            void add_factory(std::unique_ptr<abstract_plugin_factory> && factory);
			bool check_factory_presence(const unsigned int factory_id) const;

			//--------------
			const std::string& get_factory_name(const unsigned int factory_id);
			const std::string& get_factory_description(const unsigned int factory_id);

			const request_form& get_plugin_request_form(const unsigned int factory_id);

			abstract_sound_component *get_new_sound_component(
				const unsigned int factory_id, 
				data_input_stream& data, 
				const unsigned int channel_count);
			abstract_sound_component *get_new_sound_component(
				const unsigned int factory_id, 
				const answer_form& answer_form, 
				const unsigned int channel_count);
			
		//	void delete_sound_component(abstract_sound_component *component);

		private:
			abstract_plugin_factory *factory_by_id(const unsigned int factory_id) const;

            factory_deleter get_default_factory_deleter();
            factory_deleter get_plugin_factory_deleter(void *lib_handle, factory_delete_fct delete_fct);

            std::map<
                unsigned int,
                std::unique_ptr<abstract_plugin_factory, factory_deleter> > m_plugin_factory;
		};

	}

}

#endif
