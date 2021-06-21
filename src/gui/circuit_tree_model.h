#ifndef GAMMOU_CIRCUIT_TREE_MODEL_H_
#define GAMMOU_CIRCUIT_TREE_MODEL_H_

#include <vector>

#include "view.h"
#include "helpers/alphabetical_compare.h"
#include "synthesizer/synthesizer.h"

namespace Gammou {

    class circuit_tree :
        public View::abstract_storage_directory_model<std::string, std::weak_ptr<View::widget>, View::alphabetical_compare, circuit_tree>
    {
        using implem =
            View::abstract_storage_directory_model<std::string, std::weak_ptr<View::widget>, View::alphabetical_compare, circuit_tree>;
    public:
        circuit_tree(const std::weak_ptr<View::widget> ed = {}, synthesizer::circuit_controller *circuit_ctl = nullptr)
        : _config_widget{ed}, _circuit_controller{circuit_ctl}
        {}

        /**
         * \brief Get the configuration widget linked to this node
         */
        std::weak_ptr<View::widget> get_config_widget();

        /**
         * \brief Insert a config directory with an available name
         * \param desired_name name to be used if available, is suffixed if not
         * \param dir the config directory to be added
         */
        circuit_tree& insert_config_dir(std::string& desired_name, circuit_tree&& dir);

        /**
         * \brief rename a configuration
         * \param config_dir previously inserted directory to be renamed
         * \param new_name name to be applied
         */
        circuit_tree& rename_config(circuit_tree& config_dir, const std::string& new_name);

        /**
         * \brief Remove a previously inserted configuration directory
         * \param config configuration dir to be removed
         */
        void remove_config(circuit_tree& config);

        /**
         * \brief Return the circuit controller linked to this config.
         * Return null if no controller was linked
         */
        synthesizer::circuit_controller *get_circuit_controller() const noexcept;

    private:
        circuit_tree* _find_parent(const circuit_tree& child, std::string& key);

        synthesizer::circuit_controller *_circuit_controller{nullptr};
        std::weak_ptr<View::widget> _config_widget;
    };



}

#endif /* GAMMOU_CIRCUIT_TREE_MODEL_H_ */