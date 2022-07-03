

#include <fstream>
#include <DSPJIT/log.h>

#include "desktop_application.h"
#include "builtin_plugins/load_builtin_plugins.h"
#include "gui/control_node_widgets/load_control_plugins.h"
#include "helpers/alphabetical_compare.h"
#include "helpers/layout_builder.h"
#include "plugin_system/package_loader.h"
#include "synthesizer/midi_parser.h"

namespace Gammou {

    desktop_application::desktop_application(
        const configuration& config)
    : _synthesizer{_llvm_context, config.synthesizer_config}
    {
        // midi multiplex
        _initialize_midi_multiplex();

        // additional toolbox
        const View::layout_builder builder{};

        auto additional_toolbox =
            builder.horizontal<false>(
                builder.header(_make_midi_device_widget()),
                builder.header(_make_audio_device_widget()),
                builder.header(_make_debug_toolbox())
            );

        // initialize application
        _application =
            std::make_unique<application>(
                config.application_config,
                _synthesizer,
                std::move(additional_toolbox));

        if (config.initial_path.has_value())
        {
            const auto& patch_path = config.initial_path.value();
            try
            {
                nlohmann::json json;
                std::ifstream stream{patch_path, std::ios_base::in};

                if (!stream.good())
                    throw std::invalid_argument("Unable to open initial patch file");
                else
                    stream >> json;

                _application->deserialize(json);
            }
            catch (std::exception& error)
            {
                LOG_ERROR("[desktop application] Unable to load initial patch: '%s'\n",
                    patch_path.generic_string().c_str());
            }
        }

        //  display
        _display = View::create_application_display(_application->main_gui(), 1);
    }

    desktop_application::~desktop_application()
    {
        _stop_audio();
    }

    void desktop_application::open_display()
    {
        _display->open("Gammou");
    }
    void desktop_application::close_display()
    {
        _display->close();
    }

    void desktop_application::wait_display()
    {
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
            }
            catch(...)
            {
                LOG_ERROR("[desktop_application][_stop_audio] Failed to stop audio\n");
            }
            _audio_device.reset();
        }
    }

    bool desktop_application::_ignore_api(RtAudio::Api api)
    {
        // The ALSA API is slowing down the startup too much
        return (api == RtAudio::LINUX_ALSA);
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
            if(_ignore_api(api))
                continue;

            RtAudio rt_audio{api};
            auto& api_dir = audio_device_tree->get_or_create_directory(RtAudio::getApiDisplayName(api));
            const auto device_count = rt_audio.getDeviceCount();

            for (auto idx = 0u; idx <  device_count; ++idx) {
                const auto info = rt_audio.getDeviceInfo(idx);

                if (info.probed && info.outputChannels == _synthesizer.get_output_count()) {
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
            View::make_directory_view(std::move(audio_device_tree), 300, 50);

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

        midi_settings_widget->resize(300, line_height * std::max(1u, midi_input_count) + y_start_offset * 2);
        return midi_settings_widget;
    }

    std::unique_ptr<View::widget> desktop_application::_make_debug_toolbox()
    {
        // Enable/disable ir dump on logs
        auto dump_ir_box = std::make_unique<View::checkbox>();
        dump_ir_box->set_callback(
            [this](bool checked)
            {
                LOG_INFO("[desktop application] %s ir code dump\n",
                    checked ? "enable" : "disable");
                _synthesizer.enable_ir_dump(checked);
            });

        View::layout_builder builder{};
        return builder.vertical(
            builder.horizontal(
                std::move(dump_ir_box),
                std::make_unique<View::label>("Enable ir dump")),
            builder.empty_space());
    }

}