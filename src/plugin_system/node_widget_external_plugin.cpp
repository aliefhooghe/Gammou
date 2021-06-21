
#include <algorithm>
#include "node_widget_external_plugin.h"
#include "ir_loader.h"

namespace Gammou {

    void from_json(
        const nlohmann::json& j,
        node_widget_external_plugin_descriptor& desc)
    {
        j.at("name").get_to(desc.name);
        j.at("uid").get_to(desc.plugin_id);
        j.at("category").get_to(desc.category);
        j.at("input-names").get_to(desc.input_names);
        j.at("output-names").get_to(desc.output_names);
        j.at("modules").get_to(desc.modules_paths);
    }

    node_widget_external_plugin::node_widget_external_plugin(
        const node_widget_factory::plugin_id plugin_id,
        const std::string& name, const std::string& category,
        const std::vector<std::string>& input_names,
        const std::vector<std::string>& output_names,
        std::unique_ptr<llvm::Module>&& module)
    :   node_widget_factory::plugin{plugin_id, name, category},
        _dsp_plugin{std::move(module)}
    {
        const auto& proc_info = _dsp_plugin.get_process_info();
        auto ic = std::min<unsigned int>(proc_info.input_count, input_names.size());
        auto oc = std::min<unsigned int>(proc_info.output_count, output_names.size());
        _node_input_names.assign(std::begin(input_names), std::begin(input_names) + ic);
        _node_output_names.assign(std::begin(output_names), std::begin(output_names) + oc);
    }

    std::unique_ptr<node_widget_external_plugin> node_widget_external_plugin::from_desc(node_widget_external_plugin_descriptor& desc, llvm::LLVMContext& ctx)
    {
        auto module = load_ir_modules(ctx, desc.modules_paths);
        return std::make_unique<node_widget_external_plugin>(
            desc.plugin_id, desc.name, desc.category,
            desc.input_names, desc.output_names, std::move(module));
    }

    std::unique_ptr<plugin_node_widget> node_widget_external_plugin::create_node(circuit_tree&)
    {
        auto node =  std::make_unique<plugin_node_widget>(name(), id(),  _dsp_plugin.create_node());

        for (auto i = 0u; i < _node_input_names.size(); ++i)
            node->set_input_name(i, _node_input_names[i]);

        for (auto i = 0; i < _node_output_names.size(); ++i)
            node->set_output_name(i, _node_output_names[i]);

        return node;
    }

    std::unique_ptr<llvm::Module> node_widget_external_plugin::module()
    {
        return _dsp_plugin.create_module();
    }

}