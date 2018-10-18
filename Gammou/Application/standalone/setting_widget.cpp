
#include <RtAudio.h>
#include "setting_widget.h"
#include "application.h"


#define SETTING_WIDGET_WIDTH 300
#define SETTING_WIDGET_HEIGHT 500

namespace Gammou
{

    namespace Standalone
    {

        static std::string str_of_rtaudio_api(const RtAudio::Api api)
        {
            switch (api) {
                case RtAudio::LINUX_ALSA:        return "Alsa";
                case RtAudio::LINUX_PULSE:       return "PulseAudio";
                case RtAudio::UNIX_JACK:         return "Jack";
                case RtAudio::MACOSX_CORE:       return "CoreAudio";
                case RtAudio::WINDOWS_WASAPI:    return "Wasapi";
                case RtAudio::WINDOWS_ASIO:      return "Asio";
                case RtAudio::WINDOWS_DS:        return "DirectSound";
                default:                         return "Unknown";   //  should not happen
            }
        }

        setting_widget::setting_widget(application& app)
        :   View::window_widget(SETTING_WIDGET_WIDTH, SETTING_WIDGET_HEIGHT),
            m_app(app)
        {
            m_devices = 
                std::make_unique<model>();

            //  Listing devices

            std::vector<RtAudio::Api> apis;
            RtAudio::getCompiledApi(apis);

            for (auto& api : apis) {
                
                //  Add api directory
                auto& api_dir = 
                    m_devices->add_directory(str_of_rtaudio_api(api));   

                //  List devices
                RtAudio audio{api};

                const unsigned int device_count = 
                    audio.getDeviceCount();

                for (unsigned int i = 0; i < device_count; ++i) {
                    
                    //  Add device directory
                    const auto info = 
                        audio.getDeviceInfo(i);

                    auto& device_dir = 
                        api_dir.add_directory(info.name);

                    //  List samples rates

                    for (auto sample_rate : info.sampleRates) {
                        value val{api, i, sample_rate};
                        device_dir.add_value(
                            "at " + std::to_string(sample_rate) + "Hz",
                            val);
                    }
                }
            }

            //  Create directory_view
            auto device_view = 
                std::make_unique<View::directory_view<value> >(
                    *m_devices, 
                    0, 0, 
                    SETTING_WIDGET_WIDTH,
                    SETTING_WIDGET_HEIGHT,
                    20);

            device_view->set_value_select_event(
                [this](auto&, const std::string&, const value &v)
                {
                    m_app.start_audio(
                        std::get<0>(v), 
                        std::get<1>(v),
                        std::get<2>(v));
                });

            add_widget(std::move(device_view));
        }

        setting_widget::~setting_widget()
        {}

    } /* Standalone */
} /* Gammou */
