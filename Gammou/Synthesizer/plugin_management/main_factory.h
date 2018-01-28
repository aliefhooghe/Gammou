
#include <map>
#include "abstract_plugin_factory.h"



#define GAMMOU_MAKE_FACTORY_SYMBOL "make_factory"
#define GAMMOU_DELETE_FACTORY_SYMBOL "delete_factory"

namespace Gammou {

	namespace Sound {

		class main_factory {

			typedef void (*factory_delete_fct)(abstract_plugin_factory*);
			typedef abstract_plugin_factory *(*factory_make_fct)(void);

			struct plugin_lib {
				void *lib_handle;
				factory_delete_fct factory_delete;
			};

		public:
			main_factory();
			~main_factory();

			//-------------
			unsigned int load_factory(const std::string& file_path);
			void register_factory(abstract_plugin_factory *factory);

			//--------------
			const std::string& get_factory_name(const unsigned int factory_id);
			const std::string& get_factory_description(const unsigned int factory_id);

			const request_form_descriptor& get_plugin_request_form(const unsigned int factory_id) const;

			abstract_sound_component *get_new_sound_component(const unsigned int factory_id, data_source& data, const unsigned int channel_count);
			abstract_sound_component *get_new_sound_component(const unsigned int factory_id, const answer_form_descriptor& answer_form, const unsigned int channel_count);
			
			void delete_sound_component(abstract_sound_component *component);

		private:
			abstract_plugin_factory *factory_by_id(const unsigned int factory_id) const;
		
			std::map<unsigned int, std::pair<plugin_lib, abstract_plugin_factory*> > m_plugin_factory;
		};

	}

}