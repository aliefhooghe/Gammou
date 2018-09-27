
#include "component_selector.h"
#include "gui_properties.h"

namespace Gammou {

    namespace Gui {

        component_selector::component_selector(
            const int x,
            const int y,
            const unsigned int width,
            const unsigned int height,
            const unsigned int displayed_cell_count,
            gui_component_main_factory& factory)
        :   View::storage_directory_model<std::string, unsigned int>(),
            View::directory_view<unsigned int>(
                *this, x, y, width, height,
                displayed_cell_count,
                GuiProperties::main_gui_list_box_selected_item_color,
                GuiProperties::main_gui_list_box_hovered_item_color,
                GuiProperties::main_gui_list_box_background,
                GuiProperties::main_gui_list_box_font_color,
                GuiProperties::main_gui_component_choice_box_font_size),
            m_factory(factory)
        {

        }

        component_selector::~component_selector()
        {

        }

        void component_selector::add_plugin_factory(
            std::unique_ptr<Sound::abstract_plugin_factory> && factory)
        {
            add_factory(
                factory->get_name(),
                factory->get_category(),
                factory->get_factory_id());

            m_factory.add_plugin_factory(std::move(factory));
        }

        void component_selector::add_control_factory(
            std::unique_ptr<abstract_gui_component_factory> && factory)
        {
            add_factory(
                factory->get_name(),
                factory->get_category(),
                factory->get_factory_id());

            m_factory.add_complete_factory(std::move(factory));
        }
		
        void component_selector::load_plugin_factory(const std::string& path)
        {
            const unsigned int factory_id = 
                m_factory.load_plugin_factory(path);

            DEBUG_PRINT("Loaded Factory : id = %u\n", factory_id);

            const auto& name =
                m_factory.get_factory_name(factory_id);

            const auto& category = 
                m_factory.get_factory_category(factory_id);
                
            add_factory(
                name, category, factory_id);
        }

        void component_selector::add_factory(
            const std::string& name, 
            const std::string& category, 
            const unsigned int factory_id)
        {
            const auto item = get_item(category);

            if (item == nullptr) {
                auto& dir = add_directory(category);
                dir.add_value(name, factory_id);
            }
            else if(is_directory(*item)) {
                using directory = directory_model<std::string, unsigned int>::directory;
                auto& dir = std::get<directory>(*item);
                dir->add_value(name, factory_id);
            }
            else {
                throw std::runtime_error("component selector : Unable to create category");
            }
        }

    } /* Gui */

} /* Gammou */
