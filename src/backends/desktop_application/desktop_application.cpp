
#include "desktop_application.h"
#include "gui/circuit_editor.h"

#include <test_implentations.h>

namespace Gammou {

    desktop_application::desktop_application()
    : _execution_context{_llvm_context, 1u /* instances */},
        _output_node{1u, 0u},
        _external_plugin{_llvm_context, {"../plugin/plugin.bc"}, sizeof(float)}
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
                auto& execution_context = *(DSPJIT::graph_execution_context*)user_data;
                float *output = static_cast<float*>(output_buffer);

                for (auto i = 0u; i< sample_count; ++i, output++)
                    execution_context.process(nullptr, output);

                return 0;
            };

        _audio_device->openStream(&param, nullptr, RTAUDIO_FLOAT32, 48000, &buffer_size, audio_callback, &_execution_context, nullptr);
        _audio_device->startStream();

        // gui
        auto editor = std::make_unique<Gammou::circuit_editor>(50, 30);

        auto compile_callback =
            [this]()
            {
                _execution_context.compile({}, {_output_node});
            };

        editor->insert_node_widget(2, 2, std::make_unique<node_widget>("output", _output_node));
        editor->set_circuit_changed_callback(compile_callback);

        _external_plugin.prepare_context(_execution_context);

        auto create_node_callback =
            [this]()
            {
                auto node = _external_plugin.create_node();
                return std::make_unique<owning_node_widget>("proc", std::move(node));
            };

        editor->set_create_node_callback(create_node_callback);

        //  display
        _window = std::make_unique<View::background>(std::make_unique<View::map_wrapper>(std::move(editor), 50, 30));
        _display = std::make_unique<View::native_application_display>(*_window, 12);
    }

    void desktop_application::run()
    {
        float out;
        _display->open("Gammou");
        _display->wait();
    }

}