#ifndef GAMMOU_MAIN_GUI_H_
#define GAMMOU_MAIN_GUI_H_

#include "view.h"
#include "synthesizer/synthesizer.h"
#include "plugin_system/node_widget_factory.h"

namespace Gammou {

    std::unique_ptr<View::widget> make_synthesizer_gui(
        synthesizer& ,
        node_widget_factory& factory,
        std::unique_ptr<View::widget>&& additional_toolbox = nullptr);

}

#endif