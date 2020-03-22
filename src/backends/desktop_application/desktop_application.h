#ifndef GAMMOU_DESKTOP_APPLICATION_H_
#define GAMMOU_DESKTOP_APPLICATION_H_

#include <RtAudio.h>
#include "compile_node_class.h"
#include "external_plugin.h"
#include "view.h"

namespace Gammou {

    class desktop_application /* : */{

    public:
        ~desktop_application() noexcept = default;
        desktop_application(/* IO ?? */);

        void run();


    private:
        std::unique_ptr<View::widget> _window{};
        std::unique_ptr<View::native_application_display> _display{};

        std::unique_ptr<RtAudio> _audio_device{};

        llvm::LLVMContext _llvm_context{};
        DSPJIT::graph_execution_context _execution_context;
        DSPJIT::compile_node_class _output_node;
        DSPJIT::external_plugin _external_plugin;
    };

}

#endif