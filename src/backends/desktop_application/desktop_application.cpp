
#include "desktop_application.h"

#include "backends/common/configuration.h"
#include "builtin_plugins/load_builtin_plugins.h"
#include "gui/control_node_widgets/load_control_plugins.h"
#include "helpers/alphabetical_compare.h"
#include "plugin_system/package_loader.h"
#include "synthesizer/midi_parser.h"

#ifdef GAMMOU_BENCHMARKING_MODE
#include <chrono>
#endif

namespace Gammou {

    desktop_application::desktop_application(float samplerate, unsigned int input_count, unsigned int output_count)
    : _synthesizer{_llvm_context, samplerate, input_count, output_count},
        _node_factory{_llvm_context}
    {
        // midi multiplex
        _initialize_midi_multiplex();

        //  Load builtin plugins into factory
        load_builtin_plugins(_node_factory);

        //  Load control plugins into factory
        load_control_plugins(_synthesizer, _node_factory);

        //  Load packages into factory
        load_all_packages(configuration::get_packages_directory_path(), _node_factory);

        //  Prepare synthesizer
        _synthesizer.add_library_module(_node_factory.module());

        // gui
        auto additional_toolbox = View::make_horizontal_layout(
            std::make_unique<View::header>(_make_midi_device_widget())
#ifndef GAMMOU_BENCHMARKING_MODE
            ,
            std::make_unique<View::header>(_make_audio_device_widget())
#endif
        );

        _application = std::make_unique<application>(_synthesizer, _node_factory, std::move(additional_toolbox));

        //  display
        _display = View::create_application_display(_application->main_gui(), 1);
    }

    desktop_application::~desktop_application()
    {
        _stop_audio();
    }

    void desktop_application::run()
    {
        _display->open("Gammou");

#ifdef GAMMOU_BENCHMARKING_MODE
        auto minimum_sample_per_seconds = std::numeric_limits<float>::max();
        float dummy_output[2];
        while (_display->is_open()) {
            constexpr auto count = 100000u;
            const auto start = std::chrono::steady_clock::now();
            _synthesizer.update_program();

            for (auto i = 0u; i < count; ++i)
                _synthesizer.process_sample(nullptr, dummy_output);

            const auto end = std::chrono::steady_clock::now();
            const auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
            const auto sample_per_seconds = 1000.f * static_cast<float>(count) / static_cast<float>(duration);
            std::cout << "Current speed " << std::scientific << sample_per_seconds << " samples per second\n";

            if (sample_per_seconds < minimum_sample_per_seconds)
                minimum_sample_per_seconds = sample_per_seconds;
        }
        std::cout << "Minimum speed " << minimum_sample_per_seconds << " samples per second\n";
#endif
        _display->wait();
    }

    void desktop_application::_initialize_midi_multiplex()
    {
        try {
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
        catch (std::exception& e)
        {
            LOG_ERROR("[desktop_application] Could not initialize midi multiplex : '%s'\n", e.what());
            _midi_inputs.resize(0);
        }
    }

    bool desktop_application::_enable_midi_input(unsigned int idx, bool enable)
    {
        if (idx >= _midi_input_count())
            return false;

        try {
            if (enable)
                _midi_inputs[idx].openPort(idx);
            else
                _midi_inputs[idx].closePort();
            return true;
        }
        catch(...) {
            LOG_ERROR("[desktop_application][_enable_midi_input] Failed to %s midi input %u\n",
                enable ? "enable" : "disable", idx);
            return false;
        }
    }

    unsigned int desktop_application::_midi_input_count() const noexcept
    {
        return _midi_inputs.size();
    }

    bool desktop_application::_start_audio(
            RtAudio::Api api,
            unsigned int device_index,
            unsigned int sample_rate)
    {
        _stop_audio();

        RtAudio::StreamParameters output_params;
        RtAudio::StreamOptions options{};

        unsigned int buffer_size = 512;

        output_params.deviceId = device_index;
        output_params.firstChannel = 0u;
        output_params.nChannels = _synthesizer.get_output_count();

        options.flags = RTAUDIO_MINIMIZE_LATENCY | RTAUDIO_SCHEDULE_REALTIME;
        options.streamName = "Gammou output";

        auto audio_callback =
            [](void *output_buffer, void *input_buffer, unsigned int sample_count,
                double stream_time, RtAudioStreamStatus status, void *user_data)
            {
                auto& synth = *(synthesizer*)(user_data);
                synth.update_program(); //  avoid to update for every samples
                synth.process_buffer(sample_count, nullptr, static_cast<float*>(output_buffer));
                return 0;
            };

        try {
            _audio_device = std::make_unique<RtAudio>(api);
            _audio_device->openStream(
                &output_params, nullptr, RTAUDIO_FLOAT32, sample_rate, &buffer_size, audio_callback, &_synthesizer, &options);
            _synthesizer.set_sample_rate(sample_rate);
            _audio_device->startStream();
            return true;
        }
        catch(...)
        {
            LOG_ERROR("[desktop_application][_start_audio] Failed to start audio\n");
            _stop_audio();
            return false;
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

        for (const auto api : apis) {
            RtAudio rt_audio{api};
            auto& api_dir = audio_device_tree->get_or_create_directory(RtAudio::getApiDisplayName(api));
            const auto device_count = rt_audio.getDeviceCount();

            for (auto idx = 0u; idx <  device_count; ++idx) {
                const auto info = rt_audio.getDeviceInfo(idx);

                if (info.probed) {
                    auto& device_dir = api_dir.get_or_create_directory(info.name);

                    for (auto sample_rate : info.sampleRates) {
                        auto freq_key = std::to_string(sample_rate) + " Hz";
                        //  Left pad string in order to sort from lowest to highest available frequency
                        freq_key.insert(freq_key.begin(), 9 - freq_key.length(), ' ');
                        device_dir.insert_value(freq_key, {api, idx, sample_rate});
                    }
                }
            }
        }

        //  Build a widget view of device tree
        auto view =
            View::make_directory_view(std::move(audio_device_tree), 500, 50);

        //  Set callback to select
        view->set_value_select_callback(
            [this, v = view.get()](const auto& device_desc)
            {
                if (!_start_audio(
                    std::get<0>(device_desc),
                    std::get<1>(device_desc),
                    std::get<2>(device_desc)))
                {
                    v->reset_selection();
                }
            });

        return view;
    }

    std::unique_ptr<View::widget> desktop_application::_make_midi_device_widget()
    {
        auto midi_settings_widget = std::make_unique<View::panel<>>(0.f, 0.f /* dummy sizes*/);

        const auto midi_input_count = _midi_input_count();
        constexpr auto y_start_offset = 5;
        constexpr auto line_height = 21;

        for (auto i = 0u; i < midi_input_count; ++i) {
            const auto x_offset = 5.f;
            const auto y_offset = y_start_offset + i * line_height;
            auto device_checkbox = std::make_unique<View::checkbox>();
            auto device_label = std::make_unique<View::label>(_midi_inputs[i].getPortName(i));

            device_checkbox->set_callback(
                [this, idx = i, check = device_checkbox.get()](bool checked)
                {
                    // In case of failure
                    if (!_enable_midi_input(idx, checked))
                        check->set_checked(!checked);
                });

            midi_settings_widget->insert_widget(x_offset, y_offset, std::move(device_checkbox));
            midi_settings_widget->insert_widget(x_offset * 18, y_offset, std::move(device_label));
        }

        midi_settings_widget->resize(500, line_height * std::max(1u, midi_input_count) + y_start_offset * 2);
        return midi_settings_widget;
    }

}