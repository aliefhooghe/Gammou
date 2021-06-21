
#include "circuit_tree_model.h"

namespace Gammou {


    std::weak_ptr<View::widget> circuit_tree::get_config_widget()
    {
        return _config_widget;
    }

    circuit_tree& circuit_tree::rename_config(circuit_tree& config_dir, const std::string& new_name)
    {
        std::string old_name;
        circuit_tree* parent = _find_parent(config_dir, old_name);

        if (parent == nullptr) {
            throw std::invalid_argument("Can't rename a directory wich is not in the tree");
        }
        else {
            auto& new_dir = parent->move(old_name, new_name);
            return new_dir;
        }
    }

    circuit_tree& circuit_tree::insert_config_dir(std::string& desired_name, circuit_tree&& dir)
    {
        auto new_name{desired_name};
        int sufix = 1;

        do {
            if (find(new_name) == end()) {
                desired_name = new_name;
                if (dir._circuit_controller == nullptr)
                    dir._circuit_controller = _circuit_controller;
                return insert_directory(new_name, std::move(dir));
            }
            else {
                new_name = desired_name + "-" + std::to_string(sufix++);
            }
        } while (true);
    }

    void circuit_tree::remove_config(circuit_tree& config)
    {
        std::string old_name;
        circuit_tree* parent = _find_parent(config, old_name);

        if (parent != nullptr)
            parent->erase(old_name);
    }

    synthesizer::circuit_controller* circuit_tree::get_circuit_controller() const noexcept
    {
        return _circuit_controller;
    }

    circuit_tree* circuit_tree::_find_parent(const circuit_tree& child, std::string& key)
    {
        for (auto& pair : *this) {
            auto& item = pair.second;
            if (std::holds_alternative<circuit_tree>(item)) {
                auto& sub_dir = std::get<circuit_tree>(item);
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
}
