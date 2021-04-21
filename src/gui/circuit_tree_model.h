#ifndef GAMMOU_CIRCUIT_TREE_MODEL_H_
#define GAMMOU_CIRCUIT_TREE_MODEL_H_

#include <vector>

#include "view.h"
#include "helpers/alphabetical_compare.h"

namespace Gammou {

    class circuit_tree : 
        public View::abstract_storage_directory_model<std::string, std::weak_ptr<View::widget>, View::alphabetical_compare, circuit_tree>
    {
        using implem =
            View::abstract_storage_directory_model<std::string, std::weak_ptr<View::widget>, View::alphabetical_compare, circuit_tree>;
    public:
        circuit_tree(const std::weak_ptr<View::widget> ed = {})
        : _config_widget{ed}
        {}

        std::weak_ptr<View::widget> get_config_widget();
        circuit_tree& rename_config(circuit_tree& config_dir, const std::string& new_name);
        void remove_config(circuit_tree& config);

        using item = typename implem::item;
        using implem::move;
        using implem::get_or_create_directory;
        using implem::insert_directory;
        using implem::insert_value;
        using implem::clear;
        using implem::erase;

    private:
        circuit_tree* _find_parent(const circuit_tree& child, std::string& key);

        std::weak_ptr<View::widget> _config_widget;
    };

}

#endif /* GAMMOU_CIRCUIT_TREE_MODEL_H_ */