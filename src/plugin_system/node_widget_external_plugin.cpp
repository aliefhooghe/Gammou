
#include "node_widget_external_plugin.h"

namespace Gammou {

    void from_json(
        const nlohmann::json& j,
        node_widget_external_plugin_descriptor& desc)
    {
        j.at("name").get_to(desc.name);
        j.at("uid").get_to(desc.uid);
        j.at("category").get_to(desc.category);
        j.at("mutable-state-size").get_to(desc.mutable_state_size);
        j.at("input-names").get_to(desc.input_names);
        j.at("output-names").get_to(desc.output_names);
        j.at("modules").get_to(desc.modules_paths);
    }

    node_widget_external_plugin::node_widget_external_plugin(
        llvm::LLVMContext &llvm_context,
        const node_widget_external_plugin_descriptor& desc)
    :   _dsp_plugin{llvm_context, desc.modules_paths, desc.mutable_state_size}
    {
        if (_dsp_plugin.get_input_count() != desc.input_names.size() ||
            _dsp_plugin.get_output_count() != desc.output_names.size())
        {
            throw std::runtime_error("node_widget_external_plugin ctor : I/O name does not match code !");
        }

        _name = desc.name;
        _category = desc.category;
        _node_input_names = desc.input_names;
        _node_output_names = desc.output_names;
    }

    std::unique_ptr<node_widget> node_widget_external_plugin::create_node()
    {
        auto node =  std::make_unique<owning_node_widget>(_name, _dsp_plugin.create_node());

        for (auto i = 0u; i < _dsp_plugin.get_input_count(); ++i)
            node->set_input_name(i, _node_input_names[i]);

        for (auto i = 0; i < _dsp_plugin.get_output_count(); ++i)
            node->set_output_name(i, _node_output_names[i]);

        return node;
    }

    std::unique_ptr<llvm::Module> node_widget_external_plugin::module()
    {
        return _dsp_plugin.module();
    }

}