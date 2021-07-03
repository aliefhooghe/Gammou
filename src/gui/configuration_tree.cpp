
#include "configuration_tree.h"

namespace Gammou {

    /**
     *  Configuration leaf implementation
     */
    configuration_leaf::configuration_leaf(std::shared_ptr<View::widget>&& ed, synthesizer::circuit_controller* circuit_ctl)
    : _config_widget{ std::move(ed) }, _circuit_controller{ circuit_ctl }
    {

    }

    std::weak_ptr<View::widget> configuration_leaf::get_config_widget()
    {
        return _config_widget;
    }

    synthesizer::circuit_controller* configuration_leaf::get_circuit_controller() const noexcept
    {
        return _circuit_controller;
    }

    /**
     *  Configuration tree implementation
     */

    configuration_tree::configuration_tree(std::shared_ptr<View::widget>&& ed, synthesizer::circuit_controller* circuit_ctl)
    : _config_widget{ std::move(ed) }, _circuit_controller{ circuit_ctl }
    {}

    std::weak_ptr<View::widget> configuration_tree::get_config_widget()
    {
        return _config_widget;
    }

    configuration_tree& configuration_tree::rename_config(configuration_tree& config_dir, const std::string& new_name)
    {
        std::string old_name;
        configuration_tree* parent = _find_parent(config_dir, old_name);

        if (parent == nullptr) {
            throw std::invalid_argument("Can't rename a directory wich is not in the tree");
        }
        else {
            auto& new_dir = std::get<configuration_tree>(parent->move(old_name, new_name));
            return new_dir;
        }
    }

    configuration_leaf& configuration_tree::rename_config(configuration_leaf& config_dir, const std::string& new_name)
    {
        std::string old_name;
        configuration_tree* parent = _find_parent(config_dir, old_name);

        if (parent == nullptr) {
            throw std::invalid_argument("Can't rename a leaf wich is not in the tree");
        }
        else {
            auto& new_leaf = std::get<configuration_leaf>(parent->move(old_name, new_name));
            return new_leaf;
        }
    }

    configuration_tree& configuration_tree::insert_config_dir(std::string& desired_name, configuration_tree&& dir)
    {
        _find_available_name(desired_name);
        if (dir._circuit_controller == nullptr)
            dir._circuit_controller = _circuit_controller;
        return insert_directory(desired_name, std::move(dir));
    }

    configuration_leaf& configuration_tree::insert_config_leaf(std::string& desired_name, configuration_leaf&& leaf)
    {
        _find_available_name(desired_name);
        if (leaf._circuit_controller == nullptr)
            leaf._circuit_controller = _circuit_controller;
        return insert_value(desired_name, std::move(leaf));
    }

    void configuration_tree::remove_config(configuration_tree& config)
    {
        std::string old_name;
        configuration_tree* parent = _find_parent(config, old_name);

        if (parent != nullptr)
            parent->erase(old_name);
    }

    void configuration_tree::remove_config(configuration_leaf& config)
    {
        std::string old_name;
        configuration_tree* parent = _find_parent(config, old_name);

        if (parent != nullptr)
            parent->erase(old_name);
    }

    synthesizer::circuit_controller* configuration_tree::get_circuit_controller() const noexcept
    {
        return _circuit_controller;
    }

    configuration_tree* configuration_tree::_find_parent(const configuration_tree& child, std::string& key)
    {
        for (auto& pair : *this) {
            auto& item = pair.second;
            if (std::holds_alternative<configuration_tree>(item)) {
                auto& sub_dir = std::get<configuration_tree>(item);
                if (&sub_dir == &child) {
                    key = pair.first;
                    return this;
                }
                else if (auto parent = sub_dir._find_parent(child, key)) {
                    return parent;
                }
            }
        }

        return nullptr;
    }

    configuration_tree* configuration_tree::_find_parent(const configuration_leaf& child, std::string& key)
    {
        for (auto& pair : *this) {
            auto& item = pair.second;
            if (std::holds_alternative<configuration_tree>(item)) {
                auto& sub_dir = std::get<configuration_tree>(item);
                if (auto parent = sub_dir._find_parent(child, key)) {
                    return parent;
                }
            }
            else {
                auto& leaf = std::get<configuration_leaf>(item);
                if (&leaf == &child) {
                    key = pair.first;
                    return this;
                }
            }
        }

        return nullptr;
    }

    void configuration_tree::_find_available_name(std::string& desired_name)
    {
        auto new_name{ desired_name };
        int sufix = 1;

        do {
            if (find(new_name) == end()) {
                desired_name = new_name;
                return;
            }
            else {
                new_name = desired_name + "-" + std::to_string(sufix++);
            }
        } while (true);
    }
}
