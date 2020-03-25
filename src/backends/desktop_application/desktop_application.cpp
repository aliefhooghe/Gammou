
#include "desktop_application.h"
#include "gui/main_gui.h"

namespace Gammou {

    desktop_application::desktop_application()
    : _synthesizer{_llvm_context}
    {
        RtAudio::StreamParameters param;
        unsigned int buffer_size = 512;

        param.deviceId = 0u;
        param.nChannels = 1;
        param.firstChannel = 0;

        _audio_device =
            std::make_unique<RtAudio>(RtAudio::UNIX_JACK);

        auto audio_callback =
            [](void *output_buffer, void *input_buffer, unsigned int sample_count, double streamTime, RtAudioStreamStatus status, void *user_data)
            {
                auto& synth = *(synthesizer*)(user_data);
                synth.process_buffer(sample_count, nullptr, static_cast<float*>(output_buffer));
                return 0;
            };

        _audio_device->openStream(&param, nullptr, RTAUDIO_FLOAT32, 48000, &buffer_size, audio_callback, &_synthesizer, nullptr);
        _audio_device->startStream();

        // gui
        _window = make_synthesizer_gui(_synthesizer);

        //  display
        _display = std::make_unique<View::native_application_display>(*_window, 12);
    }

    void desktop_application::run()
    {
        _display->open("Gammou");
        _display->wait();
    }

}