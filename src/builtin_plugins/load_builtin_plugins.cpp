

#include "node_widget_builtin_plugin.h"
#include "load_builtin_plugins.h"

#include "test_implentations.h" //  from DSPJIT

namespace Gammou {

    void load_builtin_plugins(node_widget_factory& factory)
    {
        factory.register_plugin(1, make_builtin_plugin<DSPJIT::add_compile_node>("Add", "Calculus"));
    }

}