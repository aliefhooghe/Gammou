#ifndef GAMMOU_LOAD_BUILTIN_PLUGIN_H_
#define GAMMOU_LOAD_BUILTIN_PLUGIN_H_

#include "plugin_system/node_widget_factory.h"

namespace Gammou {

    /**
     *  \brief Load all the builtin calculus plugins into the factory
     *  \param factory the factory in which builtin plugins must be loaded
     */
    void load_builtin_plugins(node_widget_factory& factory);

}

#endif