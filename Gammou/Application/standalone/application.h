#ifndef GAMMOU_APLICATION_H_
#define GAMMOU_APLICATION_H_

#include <mutex>
#include <synthesizer.h>
#include <synthesizer_gui.h>

#include <RtAudio.h>
#include <RtMidi.h>

#include "../audio_backend/abstract_audio_backend.h"
#include "setting_widget.h"

#define GAMMOU_STANDALONE_INPUT_COUNT 2
#define GAMMOU_STANDALONE_OUTPUT_COUNT 2
#define GAMMOU_STANDALONE_CHANNEL_COUNT 128
#define GAMMOU_STANDALONE_PARAMETER_COUNT 16

namespace Gammou {

    namespace Standalone {


        class application : 
            public AudioBackend::abstract_audio_backend {

            friend class setting_widget;

            public:
                application();
                ~application();

                void run();

                //  abstract_audio_backend override

                double get_parameter_value(
                      const unsigned int index) override;

                void set_parameter_value(
                      const unsigned int index, const double value) override;

                unsigned int get_parameter_count() override;

            private:
                void start_audio(
                    const RtAudio::Api api,
                    const unsigned int device_index,
                    const unsigned int sample_rate);

                void stop_audio();
                
                static int snd_callback(
                    void *output_buffer,
                    void *input_buffer,
                    unsigned int sample_count,
                    double streamTime,
                    RtAudioStreamStatus status,
                    void *user_data);

                std::unique_ptr<RtAudio> m_audio{};
                std::mutex m_synthesizer_mutex;
                Process::bytecode_frame_processor<double>
                    m_master_circuit_processor;
                Process::bytecode_frame_processor<double>
                    m_polyphonic_circuit_processor;
                Sound::synthesizer m_synthesizer;
        };

    }   /*  Standalone */

} /* Gammou */

#endif