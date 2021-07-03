#ifndef GAMMOU_CONFIGURATION_TREE_H_
#define GAMMOU_CONFIGURATION_TREE_H_

#include <vector>

#include "view.h"
#include "helpers/alphabetical_compare.h"
#include "synthesizer/synthesizer.h"

namespace Gammou {

    class configuration_leaf
    {
        friend class configuration_tree;
    public:
        explicit configuration_leaf(std::shared_ptr<View::widget>&& ed = {}, synthesizer::circuit_controller* circuit_ctl = nullptr);
        configuration_leaf(const configuration_leaf&) = delete;
        configuration_leaf(configuration_leaf&&) noexcept = default;

        configuration_leaf& operator= (const configuration_leaf&) = delete;
        configuration_leaf& operator= (configuration_leaf&&) = default;

        /**
         * \brief Get the configuration widget linked to this leaf
         */
        std::weak_ptr<View::widget> get_config_widget();

        /**
         * \brief Return the circuit controller linked to this config.
         * Return null if no controller was linked
         */
        synthesizer::circuit_controller* get_circuit_controller() const noexcept;

    private:
        std::shared_ptr<View::widget> _config_widget{};
        synthesizer::circuit_controller* _circuit_controller{ nullptr };
    };

    class configuration_tree :
        public View::abstract_storage_directory_model<std::string, configuration_leaf, View::alphabetical_compare, configuration_tree>
    {
        using implem =
            View::abstract_storage_directory_model<std::string, configuration_leaf, View::alphabetical_compare, configuration_tree>;
    public:
        configuration_tree(std::shared_ptr<View::widget>&& ed = {}, synthesizer::circuit_controller* circuit_ctl = nullptr);
        configuration_tree(const configuration_tree&) = delete;
        configuration_tree(configuration_tree&&) noexcept = default;

        configuration_tree& operator= (configuration_tree&) = delete;
        configuration_tree& operator= (configuration_tree&&) noexcept = default;

        /**
         * \brief Get the configuration widget linked to this node
         */
        std::weak_ptr<View::widget> get_config_widget();

        /**
         * \brief Insert a config directory with an available name
         * \param desired_name name to be used if available, is suffixed if not
         * \param dir the config directory to be added
         */
        configuration_tree& insert_config_dir(std::string& desired_name, configuration_tree&& dir);

        /**
         * \brief Insert a config leaf with an available name
         * \param desired_name name to be used if available, is suffixed if not
         * \param leaf the config leaf to be added
         */
        configuration_leaf& insert_config_leaf(std::string& desired_name, configuration_leaf&& leaf);

        /**
         * \brief rename a configuration
         * \param config_dir previously inserted directory to be renamed
         * \param new_name name to be applied
         */
        configuration_tree& rename_config(configuration_tree& config_dir, const std::string& new_name);

        /**
         * \brief rename a configuration
         * \param config_dir previously inserted directory to be renamed
         * \param new_name name to be applied
         */
        configuration_leaf& rename_config(configuration_leaf& config_dir, const std::string& new_name);

        /**
         * \brief Remove a previously inserted configuration directory
         * \param config configuration dir to be removed
         */
        void remove_config(configuration_tree& config);

        /**
         * \brief Remove a previously inserted configuration directory
         * \param config configuration leaf to be removed
         */
        void remove_config(configuration_leaf& config);

        /**
         * \brief Return the circuit controller linked to this config.
         * Return null if no controller was linked
         */
        synthesizer::circuit_controller *get_circuit_controller() const noexcept;

    private:
        configuration_tree* _find_parent(const configuration_tree& child, std::string& key);
        configuration_tree* _find_parent(const configuration_leaf& child, std::string& key);
        void _find_available_name(std::string& desired_name);

        std::shared_ptr<View::widget> _config_widget{};
        synthesizer::circuit_controller* _circuit_controller{ nullptr };
    };



}

#endif /* GAMMOU_CIRCUIT_TREE_MODEL_H_ */