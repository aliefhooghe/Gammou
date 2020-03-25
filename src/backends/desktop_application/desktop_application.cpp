
#include "desktop_application.h"
#include "gui/main_gui.h"

namespace Gammou {

    desktop_application::desktop_application(unsigned int input_count, unsigned int output_count)
    : _synthesizer{_llvm_context, input_count, output_count}
    {
        //  audio
        _start_audio(RtAudio::UNIX_JACK, 0u, 48000);

        // gui
        _window = make_synthesizer_gui(_synthesizer);

        //  display
        _display = std::make_unique<View::native_application_display>(*_window, 8);
    }

    desktop_application::~desktop_application()
    {
        _stop_audio();
    }

    void desktop_application::run()
    {
        _display->open("Gammou");
        _display->wait();
    }

    void desktop_application::_start_audio(
            RtAudio::Api api,
            unsigned int device_index,
            unsigned int sample_rate)
    {
        _stop_audio();

        RtAudio::StreamParameters output_params;
        unsigned int buffer_size = 512;

        output_params.deviceId = device_index;
        output_params.firstChannel = 0u;
        output_params.nChannels = _synthesizer.get_output_count();

        auto audio_callback =
            [](void *output_buffer, void *input_buffer, unsigned int sample_count,
                double stream_time, RtAudioStreamStatus status, void *user_data)
            {
                auto& synth = *(synthesizer*)(user_data);
                synth.process_buffer(sample_count, nullptr, static_cast<float*>(output_buffer));
                return 0;
            };

        try {
            _audio_device = std::make_unique<RtAudio>(api);
            _audio_device->openStream(
                &output_params, nullptr, RTAUDIO_FLOAT32, sample_rate, &buffer_size, audio_callback, &_synthesizer);
            _audio_device->startStream();
        }
        catch(...)
        {
            _stop_audio();
        }
    }

    void desktop_application::_stop_audio()
    {
        if (_audio_device) {
            try {
                _audio_device->stopStream();
                _audio_device.reset();
            }
            catch(...)
            {}
        }
    }

}