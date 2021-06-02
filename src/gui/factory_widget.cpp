
#include "factory_widget.h"

namespace Gammou {

    factory_widget::factory_widget(node_widget_factory& factory, float width, float height)
        : View::owning_directory_view<factory_recipe_model>(
            std::make_unique<factory_recipe_model>(),
            width, height,
            16.f,/* cell height */
            14.f /* font size */),
          _factory{factory}
    {
        set_value_select_callback([this](const auto& recipe) { _current_recipe = recipe; });
        _scan_factory();
    }

    void factory_widget::register_user_node(nlohmann::json&& state, const std::string& name)
    {
        auto& recipes = data_model();
        auto& user_node_dir = recipes.get_or_create_directory("User");
        user_node_dir.insert_value(name, factory_recipe{ std::move(state) });
        update();
    }

    std::unique_ptr<plugin_node_widget> factory_widget::create_node(circuit_tree& parent_config)
    {
        return _create_node_from_recipe(_current_recipe, parent_config);
    }

    void factory_widget::_scan_factory()
    {
        auto& recipes = data_model();

        //  Insert each plugin
        for (const auto& pair : _factory) {
            auto uid = pair.first;
            const auto& plugin = pair.second;

            //  Create or get existing category directory
            auto& category_dir = recipes.get_or_create_directory(plugin->category());

            //  Insert plugin in category directory
            category_dir.insert_value(plugin->name(), factory_recipe{uid});
        }

        update();
    }

    std::unique_ptr<plugin_node_widget> factory_widget::_create_node_from_recipe(const factory_recipe& recipe, circuit_tree& parent_config)
    {
        return std::visit(
            [this, &parent_config](const auto& r)
            {
                return _factory.create_node(r, parent_config);
            },
            recipe);
    }

}