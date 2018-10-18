#ifndef SETTING_WIDGET_H_
#define SETTING_WIDGET_H_

#include <tuple>
#include <view.h>

namespace Gammou {

    namespace Standalone {

        class application;

        class setting_widget : public View::window_widget {

            using value = 
                std::tuple<
                    RtAudio::Api, 
                    unsigned int,
                    unsigned int>;

            using model = 
                View::storage_directory_model<std::string, value>;

            public:
                setting_widget(application& app);
                ~setting_widget();

            private:
                application& m_app;
                std::unique_ptr<model> m_devices;
        };

    } /* Standalone */

} /* Gammou */

#endif