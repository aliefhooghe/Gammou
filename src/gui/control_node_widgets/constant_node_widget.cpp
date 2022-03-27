#include <sstream>
#include <DSPJIT/common_nodes.h>
#include "constant_node_widget.h"

namespace Gammou
{
    static constexpr auto constant_node_widget_uid = 0x3feb405c9167036eu;

    class constant_node_widget : public plugin_node_widget
    {
    public:
        constant_node_widget(float initial_value = 0.f)
        : plugin_node_widget{
            "Constant", constant_node_widget_uid, _create_constant_node(initial_value)}
        {
            const auto text_input_width = node_width - 2.f * node_widget::node_header_size;
            auto text_input = std::make_unique<View::text_input>(text_input_width);

            // Pretty print the initial value
            std::stringstream ss;
            ss << initial_value;
            text_input->set_text(ss.str());

            text_input->set_enter_callback(
                [this, input = text_input.get()]()
                {
                    const auto input_value = _safe_str2float(input->get_text());
                    _constant_node->set_value(input_value);
                });

            set_output_name(0u, "");
            resize_height(node_widget::node_header_size * 2.f + text_input->height());
            insert_widget(
                node_widget::node_header_size, node_widget::node_header_size * 1.5f,
                std::move(text_input));
        }

    protected:
        nlohmann::json serialize_internal_state() override
        {
            const auto current_value = _constant_node->get_value();
            return { { "value", current_value } };
        }

    private:

        std::unique_ptr<DSPJIT::constant_node> _create_constant_node(float initial_value)
        {
            auto constant_node = std::make_unique<DSPJIT::constant_node>(initial_value);
            _constant_node = constant_node.get();
            return std::move(constant_node);
        }

        static float _safe_str2float(const std::string& str)
        {
            try
            {
                return std::stof(str);
            }
            catch(const std::invalid_argument&)
            {
                return 1.f;
            }
            catch(const std::out_of_range&)
            {
                return 1.f;
            }
        }

        DSPJIT::constant_node *_constant_node;
    };

    constant_node_widget_plugin::constant_node_widget_plugin()
    :   node_widget_factory::plugin{constant_node_widget_uid, "Constant", "Control"}
    {
    }

    std::unique_ptr<plugin_node_widget> constant_node_widget_plugin::create_node(abstract_configuration_directory&)
    {
        return std::make_unique<constant_node_widget>(1.f);
    }

    std::unique_ptr<plugin_node_widget> constant_node_widget_plugin::create_node(abstract_configuration_directory&, const nlohmann::json& internal_state)
    {
        const auto initial_value = internal_state["value"].get<float>();
        return std::make_unique<constant_node_widget>(initial_value);
    }
}
