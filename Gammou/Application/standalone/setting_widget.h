#ifndef SETTING_WIDGET_H_
#define SETTING_WIDGET_H_

#include <RtAudio.h>
#include <tuple>
#include <view.h>

namespace Gammou {

    namespace Standalone {

        class application;

        class setting_widget : public View::window_widget {

            using audio_device_idx =
                std::tuple<
                    RtAudio::Api,
                    unsigned int,
                    unsigned int>;

            using audio_device_list =
                View::storage_directory_model<std::string, audio_device_idx>;

            public:
                setting_widget(application& app);
                ~setting_widget();

            private:
                application& m_app;
                std::unique_ptr<audio_device_list> m_devices;
        };

    } /* Standalone */

} /* Gammou */

#endif