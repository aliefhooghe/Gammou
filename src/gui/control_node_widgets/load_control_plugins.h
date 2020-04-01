#ifndef GAMMOU_LOAD_CONTROL_PLUGINS_H_
#define GAMMOU_LOAD_CONTROL_PLUGINS_H_

#include "plugin_system/node_widget_factory.h"
#include "synthesizer/synthesizer.h"

namespace Gammou {

    /**
     *  \brief Load all the control plugins into the factory
     *  \param synthesizer which will be controlled by the loaded control plugins
     *  \param factory the factory in which control plugins must be loaded
     */
    void load_control_plugins(synthesizer& synth, node_widget_factory& factory);

}


#endif