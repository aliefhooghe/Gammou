
#include "internal_node_widget.h"

namespace Gammou {

    internal_node_widget::internal_node_widget(
        const std::string& name,
        const std::string& identifier,
        DSPJIT::compile_node_class& node)
    :   node_widget{name, node},
        _identifier{identifier}
    {
        set_internal(true);
    }

    nlohmann::json internal_node_widget::serialize()
    {
        return _identifier;
    }

}