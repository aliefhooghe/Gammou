#ifndef COMPONENT_SELECTOR_H_
#define COMPONENT_SELECTOR_H_

#include <string>
#include <view.h>
#include "gui_component_main_factory.h"

namespace Gammou {

    namespace Gui {

        class component_selector : 
            private View::storage_directory_model<std::string, unsigned int>,
            public View::directory_view<unsigned int> {

            public:
                component_selector(
                    const int x,
                    const int y,
                    const unsigned int width,
                    const unsigned int height,
                    const unsigned int displayed_cell_count,
                    gui_component_main_factory& factory);

                ~component_selector();

                void add_plugin_factory(std::unique_ptr<Sound::abstract_plugin_factory> && factory);
                void add_control_factory(std::unique_ptr<abstract_gui_component_factory> && factory);
			    void load_plugin_factory(const std::string& path);

            private:
                void add_factory(
                    const std::string& name, 
                    const std::string& category, 
                    const unsigned int factory_id);

                gui_component_main_factory& m_factory;
        };

    } /* Gui */

} /* Gammou */

#endif
