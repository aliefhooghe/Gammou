

#include "node_widget_builtin_plugin.h"
#include "load_builtin_plugins.h"

#include "common_nodes.h" //  from DSPJIT

namespace Gammou {

    void load_builtin_plugins(node_widget_factory& factory)
    {
        factory.register_plugin(make_builtin_plugin<DSPJIT::add_node>(0xdafa0fde10153761u, "Add", "Calculus"));
        factory.register_plugin(make_builtin_plugin<DSPJIT::mul_node>(0x5d777d71620078eeu, "Mul", "Calculus"));
        factory.register_plugin(make_builtin_plugin<DSPJIT::invert_node>(0xe8c8aa483f24617c, "Invert", "Calculus"));
    }

}