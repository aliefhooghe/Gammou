#ifndef GAMMOU_VST_PLUGIN_H_
#define GAMMOU_VST_PLUGIN_H_

#include "synthesizer/synthesizer.h"
#include "gui/main_gui.h"
#include "vst_compat.hpp"
#include <view.h>

namespace Gammou {

    class vst2_plugin
    {
    public:
        static AEffect* create_AEffect_instance(audioMasterCallback master);
        ~vst2_plugin() = default;

    private:
        vst2_plugin(audioMasterCallback master);

        /*
         *      VST callbacks
         */

        static intptr_t dispatcher_proc(AEffect* fx,
                                        int32_t opcode,
                                        int32_t index,
                                        intptr_t value,
                                        void* ptr,
                                        float opt);

        static void set_parameter_proc(AEffect* fx, int32_t index, float value);
        static float get_parameter_proc(AEffect* fx, int32_t index);

        static void process_replacing_proc(AEffect* fx,
                                            float** inputs,
                                            float** outputs,
                                            int32_t sample_count);
        /*
         *      helpers
         */
        void _handle_event(const VstEvent& ev);
        std::size_t _load_state(const void *chunk, std::size_t size);
        std::size_t _save_state(void **chunk_ptr);

        //  Vst2 plugin instance
        AEffect* _effect;

        //  Processing
        llvm::LLVMContext _llvm_context{};
        synthesizer _synthesizer;

        //  Gui
        std::unique_ptr<main_gui> _main_gui;
        std::unique_ptr<View::native_vst2_display> _display;
        ERect _window_rect; //  used by host

        //  Widget node factory
        node_widget_factory _node_factory;
    };


}

#endif