#ifndef GAMMOU_DESKTOP_APPLICATION_H_
#define GAMMOU_DESKTOP_APPLICATION_H_

#include <optional>
#include <RtAudio.h>
#include <RtMidi.h>

#include "synthesizer/synthesizer.h"
#include "application/application.h"

#include <view.h>

namespace Gammou {

    class midi_device_widget;

    class desktop_application {
        friend class midi_device_widget;
    public:

        struct configuration
        {
            synthesizer::configuration synthesizer_config{};
            application::configuration application_config{};
            std::optional<std::filesystem::path> initial_path{};
        };

        desktop_application(
            const configuration& config);
        ~desktop_application();

        void open_display();
        void wait_display();
        void close_display();

    private:
        void _initialize_midi_multiplex();
        bool _enable_midi_input(unsigned int idx, bool enable = true);
        unsigned int _midi_input_count() const noexcept;

        bool _start_audio(
            RtAudio::Api api,
            unsigned int device_index,
            unsigned int sample_rate);
        void _stop_audio();
        bool _ignore_api(RtAudio::Api);

        // toolbox construction
        std::unique_ptr<View::widget> _make_audio_device_widget();
        std::unique_ptr<View::widget> _make_midi_device_widget();
        std::unique_ptr<View::widget> _make_debug_toolbox();

        /*
         *  Members
         */

        //  Processing
        llvm::LLVMContext _llvm_context;
        synthesizer _synthesizer;

        //  GUI
        std::unique_ptr<application> _application;
        std::unique_ptr<View::application_display> _display{};

        //  Audio I/O
        std::unique_ptr<RtAudio> _audio_device{};

        //  Midi inputs
        std::vector<RtMidiIn> _midi_inputs{};
    };

}

#endif