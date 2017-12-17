#ifndef ABSTRACT_PLUGIN_FACTORY_H_
#define ABSTRACT_PLUGIN_FACTORY_H_


namespace Gammou {

    namespace Sound {

        class abstract_plugin_factory{

        public:
			abstract_plugin_factory(const unsigned int factory_id);
            

			/*
			
			module create_module_from_state_data(data);
			module create_new_module(request_mgr);

			*/


        private:
			const unsigned int m_factory_id;
        };
		
    } /* Sound */

} /* Gammou */



#endif