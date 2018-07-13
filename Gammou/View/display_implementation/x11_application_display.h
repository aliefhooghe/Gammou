#ifndef X11_APPLICATION_DISPLAY_H_
#define X11_APPLICATION_DISPLAY_H_

#include "display/abstract_x11_display.h"
#include "display/abstract_application_display.h"

namespace Gammou {

    namespace View {

        class x11_application_display : 
            public abstract_x11_display,
            public abstract_application_display {

        public:
            x11_application_display(View::widget& root_widget);
            x11_application_display(abstract_display &parent, View::widget& root_widget);
            ~x11_application_display();

            void open(const std::string& title) override;
            void wait();
        };

    }   /* Gammou */

}   /* Gammou */

#endif