
#include "desktop_application.h"

#include "backends/common/configuration.h"
#include "builtin_plugins/load_builtin_plugins.h"
#include "gui/control_node_widgets/load_control_plugins.h"
#include "gui/main_gui.h"
#include "helpers/alphabetical_compare.h"
#include "plugin_system/package_loader.h"
#include "synthesizer/midi_parser.h"

#ifdef GAMMOU_BENCHMARKING_MODE
#include <chrono>
#endif

namespace Gammou {

    desktop_application::desktop_application(unsigned int input_count, unsigned int output_count)
    : _synthesizer{_llvm_context, input_count, output_count},
        _node_factory{_llvm_context}
    {
        // midi multiplex
        _initialize_midi_multiplex();

        //  Load builtin plugins into factory
        load_builtin_plugins(_node_factory);

        //  Load control plugins into factory
        load_control_plugins(_synthesizer, _node_factory);

        //  Load packages into factory
        load_all_packages(get_packages_directory_path(), _node_factory);

        //  Prepare synthesizer
        _synthesizer.add_module(_node_factory.module());

        // gui
        auto additional_toolbox = View::make_horizontal_layout(
            std::make_unique<View::header>(_make_midi_device_widget())
#ifndef GAMMOU_BENCHMARKING_MODE
            ,
            std::make_unique<View::header>(_make_audio_device_widget())
#endif
        );

        _main_gui = std::make_unique<main_gui>(
            _synthesizer, _node_factory, std::move(additional_toolbox));

        //  display
        _display = std::make_unique<View::native_application_display>(_main_gui->widget(), 12);
    }

    desktop_application::~desktop_application()
    {
        _stop_audio();
    }

    void desktop_application::run()
    {
        _display->open("Gammou");

#ifdef GAMMOU_BENCHMARKING_MODE
        float dummy_output[2];
        while (_display->is_open()) {
            constexpr auto count = 100000u;
            const auto start = std::chrono::steady_clock::now();

            for (auto i = 0u; i < count; ++i)
                _synthesizer.process_sample(nullptr, dummy_output);

            const auto end = std::chrono::steady_clock::now();
            const auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
            const auto sample_per_seconds = 1000.f * static_cast<float>(count) / static_cast<float>(duration);
            std::cout << "Current speed " << std::scientific << sample_per_seconds << " samples per second\n";
        }
#endif
        _display->wait();
    }

    void desktop_application::_initialize_midi_multiplex()
    {
        RtMidiIn rt_midi;
        const auto midi_input_count = rt_midi.getPortCount();

        //  We need one RtMidi instance per input port
        _midi_inputs.resize(midi_input_count);

        auto midi_callback =
            [](double timestamp, std::vector<unsigned char> *message, void *user_data)
            {
                auto& synth = *(synthesizer*)(user_data);
                execute_midi_msg(synth, message->data(), message->size());
            };

        for (auto& input : _midi_inputs)
            input.setCallback(midi_callback, &_synthesizer);
    }

    void desktop_application::_enable_midi_input(unsigned int idx, bool enable)
    {
        if (idx >= _midi_input_count())
            return;

        try {
            if (enable)
                _midi_inputs[idx].openPort(idx);
            else
                _midi_inputs[idx].closePort();
        }
        catch(...) {
            LOG_ERROR("[desktop_application][_enable_midi_input] Failed to %s midi input %u\n",
                enable ? "enable" : "disable", idx);
        }
    }

    unsigned int desktop_application::_midi_input_count() const noexcept
    {
        return _midi_inputs.size();
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
            LOG_ERROR("[desktop_application][_start_audio] Failed to start audio\n");
            _stop_audio();
        }
    }

    void desktop_application::_stop_audio()
    {
        if (_audio_device) {
            try {
                if (_audio_device->isStreamRunning())
                    _audio_device->stopStream();
                if (_audio_device->isStreamOpen())
                    _audio_device->closeStream();
                _audio_device.reset();
            }
            catch(...)
            {
                LOG_ERROR("[desktop_application][_stop_audio] Failed to stop audio\n");
            }
        }
    }

    static auto rt_audio_api_to_str(RtAudio::Api api)
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

    std::unique_ptr<View::widget> desktop_application::_make_audio_device_widget()
    {
        //  The model used to describe available audio devices
        using audio_device_descriptor =
            std::tuple<
                RtAudio::Api, // api
                unsigned int, // device_index
                unsigned int>; // sample_rate

        using model =
            View::storage_directory_model<std::string, audio_device_descriptor, View::alphabetical_compare>;

        auto audio_device_tree = std::make_unique<model>();

        //  List every available audio device and build device tree
        std::vector<RtAudio::Api> apis;
        RtAudio::getCompiledApi(apis);

        for (const auto& api : apis) {
            RtAudio rt_audio{api};
            auto& api_dir = audio_device_tree->add_directory(rt_audio_api_to_str(api));
            const auto device_count = rt_audio.getDeviceCount();

            for (auto idx = 0u; idx <  device_count; ++idx) {
                const auto info = rt_audio.getDeviceInfo(idx);
                auto& device_dir = api_dir.add_directory(info.name);

                for (auto sample_rate : info.sampleRates) {
                    auto freq_key = std::to_string(sample_rate) + " Hz";
                    //  Left pad string in order to sort from lowest to highest available frequency
                    freq_key.insert(freq_key.begin(), 9 - freq_key.length(), ' ');
                    device_dir.add_value(freq_key, {api, idx, sample_rate});
                }
            }
        }

        //  Build a widget view of device tree
        auto view =
            View::make_directory_view(std::move(audio_device_tree), 10, 5);

        //  Set callback to select
        view->set_value_select_callback(
            [this](const auto& device_desc)
            {
                _start_audio(
                    std::get<0>(device_desc),
                    std::get<1>(device_desc),
                    std::get<2>(device_desc));
            });

        return view;
    }

    std::unique_ptr<View::widget> desktop_application::_make_midi_device_widget()
    {
        auto midi_settings_widget = std::make_unique<View::panel<>>(0.f, 0.f /* dummy sizes*/);

        const auto midi_input_count = _midi_input_count();
        constexpr auto y_start_offset = 0.5f;
        constexpr auto line_height = 1.2f;

        for (auto i = 0u; i < midi_input_count; ++i) {
            const auto y_offset = y_start_offset + i * line_height;
            auto device_checkbox = std::make_unique<View::checkbox>();
            auto device_label = std::make_unique<View::label>(_midi_inputs[i].getPortName(i));

            device_checkbox->set_callback(
                [this, idx = i](bool checked)
                {
                    _enable_midi_input(idx, checked);
                });

            midi_settings_widget->insert_widget(1.0, y_offset + 0.1, std::move(device_checkbox));
            midi_settings_widget->insert_widget(2.2f, y_offset, std::move(device_label));
        }

        midi_settings_widget->resize(30, line_height * midi_input_count + y_start_offset * 2);

        return midi_settings_widget;
    }

}