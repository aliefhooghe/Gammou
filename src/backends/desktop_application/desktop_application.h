#ifndef GAMMOU_DESKTOP_APPLICATION_H_
#define GAMMOU_DESKTOP_APPLICATION_H_

#include <RtAudio.h>
#include "synthesizer/synthesizer.h"
#include "view.h"

namespace Gammou {

    class desktop_application /* : */{

    public:
        ~desktop_application() noexcept = default;
        desktop_application(/* IO ?? */);

        void run();
    private:
        //  Processing
        llvm::LLVMContext _llvm_context;
        synthesizer _synthesizer;

        //  GUI
        std::unique_ptr<View::widget> _window{};
        std::unique_ptr<View::native_application_display> _display{};

        //  Audio
        std::unique_ptr<RtAudio> _audio_device{};
    };

}

#endif