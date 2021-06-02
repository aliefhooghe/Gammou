#ifndef GAMMOU_FACTORY_WIDGET_H_
#define GAMMOU_FACTORY_WIDGET_H_

#include <view.h>
#include <variant>
#include <plugin_system/node_widget_factory.h>

namespace Gammou {

    using factory_recipe =
        std::variant<
            node_widget_factory::plugin_id,
            nlohmann::json
        >;

    using factory_recipe_model =
        View::storage_directory_model<std::string, factory_recipe, View::alphabetical_compare>;

    class factory_widget : public View::owning_directory_view<factory_recipe_model> {

    public:
        factory_widget(node_widget_factory& factory, float width, float height);
        factory_widget(const factory_widget&) = delete;

        void register_user_node(nlohmann::json&& state, const std::string&);
        std::unique_ptr<plugin_node_widget> create_node(circuit_tree& parent_config);
    private:
        void _scan_factory();
        std::unique_ptr<plugin_node_widget> _create_node_from_recipe(const factory_recipe&, circuit_tree& parent_config);

        node_widget_factory& _factory;
        factory_recipe _current_recipe{node_widget_factory::no_id};
    };

}

#endif /* GAMMOU_FACTORY_WIDGET_H_ */