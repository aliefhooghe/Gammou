#ifndef VST2_PLUGIN_H
#define VST2_PLUGIN_H_

#include <mutex>


#include "../../debug.h"
#include "vst_compat.hpp"

#include "../gui/synthesizer_gui.h"
#include "synthesizer.h"
#include "midi_driver/midi_driver.h"

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

				void handle_event(VstEvent& ev);
				void get_param_name(char *str, const unsigned int index);

				unsigned int save_state(void **data);
				unsigned load_state(void *data, const unsigned int size);

                static intptr_t dispatcher_proc(
                    AEffect *fx, 
                    int32_t opcode, 
                    int32_t index, 
                    intptr_t value, 
                    void *ptr, 
                    float opt);

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


				//-------

                AEffect *m_aeffect;
				ERect m_window_rect;

                std::mutex m_synthesizer_mutex;

			    Process::bytecode_frame_processor<double> m_master_circuit_processor;
			    Process::bytecode_frame_processor<double> m_polyphonic_circuit_processor;

                //Sound::jit_frame_processor m_master_circuit_processor;
                //Sound::jit_frame_processor m_polyphonic_circuit_processor;
			    Sound::synthesizer m_synthesizer;
                Sound::midi_driver m_midi_driver;

                Gui::synthesizer_gui m_gui;
			    View::vst2_display m_display;

				//	Using the same buffer several time avoid multiple reallocation
				Persistence::buffer_stream m_chunk_buffer;
        };

		class raw_data_source : public Sound::data_input_stream {

		public:
			raw_data_source(
				const void *raw_data, 
				const unsigned int size_limit);

			raw_data_source(raw_data_source&) = delete;
			~raw_data_source();

			bool seek(const int offset, Sound::abstract_data_stream::seek_mode mode) override;
			unsigned int tell() override;
			unsigned int read(void *data, const unsigned int size) override;

		private:
			uint8_t * const m_begin;
			const unsigned int m_size_limit;
			unsigned int m_cursor;
		};

    }   /*  vst2 */

}   /* Gammou */
#endif
