
#include "setting_widget.h"
#include "application.h"
#include "gui_properties.h"

#define SETTING_WIDGET_WIDTH 300

#define LABEL_HEIGHT 20
#define AUDIO_DEVICE_LIST_HEIGHT 200
#define MIDI_DEVICE_LIST_HEIGHT 200

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
        :   View::window_widget(
                SETTING_WIDGET_WIDTH,
                2 * LABEL_HEIGHT + AUDIO_DEVICE_LIST_HEIGHT + MIDI_DEVICE_LIST_HEIGHT,
                Gui::GuiProperties::main_gui_list_box_background),
            m_app(app)
        {

            //  Create audio device list

            m_devices =
                std::make_unique<audio_device_list>();

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
                        audio_device_idx idx{api, i, sample_rate};
                        device_dir.add_value(
                            "at " + std::to_string(sample_rate) + "Hz",
                            idx);
                    }
                }
            }

            //  Create audio directory_view
            auto audio_device_view =
                std::make_unique<View::directory_view<audio_device_idx> >(
                    *m_devices,
                    0, LABEL_HEIGHT,
                    SETTING_WIDGET_WIDTH,
                    AUDIO_DEVICE_LIST_HEIGHT,
                    8,
                    Gui::GuiProperties::main_gui_list_box_selected_item_color,
                    Gui::GuiProperties::main_gui_list_box_hovered_item_color,
                    Gui::GuiProperties::main_gui_list_box_background,
                    Gui::GuiProperties::main_gui_list_box_font_color,
                    Gui::GuiProperties::component_font_size);

            audio_device_view->set_value_select_event(
                [this](auto&, const std::string&, const audio_device_idx &v)
                {
                    m_app.start_audio(
                        std::get<0>(v),
                        std::get<1>(v),
                        std::get<2>(v));
                });

            //  Create midi list view
            auto midi_device_view =
                std::make_unique<View::list_box>(
                    0, LABEL_HEIGHT + AUDIO_DEVICE_LIST_HEIGHT + LABEL_HEIGHT,
                    SETTING_WIDGET_WIDTH,
                    MIDI_DEVICE_LIST_HEIGHT,
                    8,
                    Gui::GuiProperties::main_gui_list_box_selected_item_color,
                    Gui::GuiProperties::main_gui_list_box_hovered_item_color,
                    Gui::GuiProperties::main_gui_list_box_background,
                    Gui::GuiProperties::main_gui_list_box_font_color,
                    Gui::GuiProperties::component_font_size);

            midi_device_view->set_item_select_event(
                [this](auto&, const unsigned int idx)
                {
                    m_app.start_midi(idx);
                });

            const auto midi_port_count = m_app.m_midi.getPortCount();
            DEBUG_PRINT("[MIDI IN] %u midi devices where detected\n", midi_port_count);

            for (auto i = 0; i < midi_port_count; ++i)
                midi_device_view->add_item(m_app.m_midi.getPortName(i));

            //  Windows Layout
            add_widget(std::make_unique<View::label>(
                "Audio Devices", 0, 0, SETTING_WIDGET_WIDTH, LABEL_HEIGHT,
                Gui::GuiProperties::component_font_color, Gui::GuiProperties::component_font_size));

            add_widget(std::move(audio_device_view));

            add_widget(std::make_unique<View::label>(
                "Midi Devices", 0, LABEL_HEIGHT + AUDIO_DEVICE_LIST_HEIGHT, SETTING_WIDGET_WIDTH, LABEL_HEIGHT,
                Gui::GuiProperties::component_font_color, Gui::GuiProperties::component_font_size));

            add_widget(std::move(midi_device_view));
        }

        setting_widget::~setting_widget()
        {}

    } /* Standalone */
} /* Gammou */
