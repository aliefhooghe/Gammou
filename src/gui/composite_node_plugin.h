#ifndef GAMMOU_COMPOSITE_NODE_PLUGIN_H_
#define GAMMOU_COMPOSITE_NODE_PLUGIN_H_

#include "plugin_system/node_widget_factory.h"
#include "main_gui.h"

namespace Gammou {

    class composite_node_plugin : public node_widget_factory::plugin {
    public:
        static constexpr node_widget_factory::plugin_id uid = 0x82796d4e78cd63f1;

        composite_node_plugin(main_gui& gui);        

        std::unique_ptr<plugin_node_widget> create_node(circuit_tree_model& dir) override;
        std::unique_ptr<plugin_node_widget> create_node(circuit_tree_model& dir, const nlohmann::json& state) override;

    private:
        main_gui& _main_gui;
    };

}

#endif