

#include "node_widget_builtin_plugin.h"
#include "load_builtin_plugins.h"

#include <DSPJIT/common_nodes.h>
#include "additional_builtin_nodes.h"

namespace Gammou {

    void load_builtin_plugins(node_widget_factory& factory)
    {
        factory.register_plugin(make_builtin_plugin<DSPJIT::add_node>(0xdafa0fde10153761u, "Add", "Calculus"));
        factory.register_plugin(make_builtin_plugin<DSPJIT::substract_node>(0x354a1a7d23ee5f6e, "Sub", "Calculus"));
        factory.register_plugin(make_builtin_plugin<DSPJIT::mul_node>(0x5d777d71620078eeu, "Mul", "Calculus"));
        factory.register_plugin(make_builtin_plugin<DSPJIT::negate_node>(0xcd3d55d993e1c8d0, "Negate", "Calculus"));
        factory.register_plugin(make_builtin_plugin<logical_not_node>(0x2913f07efa7a7684, "1 - X", "Calculus"));
    }

}