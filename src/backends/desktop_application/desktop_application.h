#ifndef GAMMOU_DESKTOP_APPLICATION_H_
#define GAMMOU_DESKTOP_APPLICATION_H_

#include <RtAudio.h>
#include "synthesizer/synthesizer.h"
#include "view.h"

namespace Gammou {

    class desktop_application /* : */{

    public:
        desktop_application(unsigned int input_count, unsigned int output_count);
        ~desktop_application();

        void run();

    private:
        void _start_audio(
            RtAudio::Api api,
            unsigned int device_index,
            unsigned int sample_rate);
        void _stop_audio();

        //  Processing
        llvm::LLVMContext _llvm_context;
        synthesizer _synthesizer;

        //  GUI
        std::unique_ptr<View::widget> _window{};
        std::unique_ptr<View::native_application_display> _display{};

        //  Audio I/O
        std::unique_ptr<RtAudio> _audio_device{};
    };

}

#endif