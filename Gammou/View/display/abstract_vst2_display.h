#ifndef ABSTRACT_VST2_DISPLAY_H_
#define ABSTRACT_VST2_DISPLAY_H_

#include "abstract_display.h"

namespace Gammou {

    namespace View {

        class abstract_vst2_display : virtual public abstract_display {

            public:
                abstract_vst2_display(View::widget& root_widget);
                virtual ~abstract_vst2_display();

                virtual void open(void *host_parent_window) = 0;
        };

    }   /*  View */

} /* Gammou */

#endif