
#include "x11_application_display.h"

namespace Gammou {

    namespace View {

        x11_application_display::x11_application_display(
            View::widget& root_widget)
        :   abstract_display(root_widget),
            abstract_x11_display(root_widget),
            abstract_application_display(root_widget)
        {
        }

        x11_application_display::x11_application_display(
            x11_application_display *parent, 
            View::widget& root_widget)
        :   abstract_display(root_widget),
            abstract_x11_display(root_widget),
            abstract_application_display(root_widget)
        {
            // TODO
        }

        x11_application_display::~x11_application_display()
        {
        }

        void x11_application_display::open(const std::string& title)
        {
            create_window(nullptr, title);
        }

        void x11_application_display::wait()
        {
            wait_window_thread();
        }

    }   /* Gammou */

}   /* Gammou */