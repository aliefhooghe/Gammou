#ifndef GAMMOU_CIRCUIT_TREE_MODEL_H_
#define GAMMOU_CIRCUIT_TREE_MODEL_H_

#include "view.h"
#include "helpers/alphabetical_compare.h"

namespace Gammou {

    struct circuit_tree_model : 
        View::abstract_storage_directory_model<std::string, std::weak_ptr<View::widget>, View::alphabetical_compare, circuit_tree_model>
    {
        using implem = 
            View::abstract_storage_directory_model<std::string, std::weak_ptr<View::widget>, View::alphabetical_compare, circuit_tree_model>;

        circuit_tree_model(const std::weak_ptr<View::widget> ed = {})
        : editor{ed}
        {}

        using implem::add_directory;
        using implem::add_value;
        using implem::clear;
        using implem::erase;

        std::weak_ptr<View::widget> editor;
    };

}

#endif /* GAMMOU_CIRCUIT_TREE_MODEL_H_ */