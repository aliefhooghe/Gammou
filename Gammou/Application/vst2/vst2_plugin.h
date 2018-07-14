#ifndef VST2_PLUGIN_H
#define VST2_PLUGIN_H_

#include <mutex>


#include "../../debug.h"
#include "vst_compat.hpp"

#include "../gui/synthesizer_gui.h"
#include "synthesizer.h"
#include "jit_frame_processor/jit_frame_processor.h"

#include <view.h>

#define GAMMOU_VST2_INPUT_COUNT 2
#define GAMMOU_VST2_OUTPUT_COUNT 2
#define GAMMOU_VST2_CHANNEL_COUNT 64
#define GAMMOU_VST2_PARAMETER_COUNT 16


namespace Gammou  {
    
    namespace VST2 {

        class plugin {

            public:
                static AEffect *create_AEffect_instance();
                ~plugin();

            private:
                plugin();

                AEffect *get_AEffect_instance();

                static intptr_t dispatcher_proc(
                    AEffect *fx, 
                    int32_t opcode, 
                    int32_t index, 
                    intptr_t value, 
                    void *ptr, 
                    float opt);

                static void process_proc(
                    AEffect *fx,
                    float **inputs,
                    float **outputs,
                    int32_t sample_count);

                static void set_parameter_proc(
                    AEffect *fx, 
                    int32_t index,
                    float value);

                static float get_parameter_proc(
                    AEffect *fx,
                    int32_t index);

                static void process_replacing_proc(
                    AEffect *fx,
                    float **inputs,
                    float **outputs,
                    int32_t sample_count);

                static void process_double_replacing_proc(
                    AEffect *fx,
                    double **inputs,
                    double **outputs,
                    int32_t sample_count);

                AEffect *m_aeffect;

                std::mutex m_synthesizer_mutex;
			    Process::bytecode_frame_processor<double> m_master_circuit_processor;
			    Process::bytecode_frame_processor<double> m_polyphonic_circuit_processor;
			    Sound::synthesizer m_synthesizer;

                Gui::synthesizer_gui m_gui;
			    View::vst2_display m_display;
        };

    }   /*  vst2 */

}   /* Gammou */
#endif