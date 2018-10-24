
#include "application.h"

#define AUDIO_BUFFER_SIZE 512

namespace Gammou
{

    namespace Standalone
    {

        application::application()
        :   m_synthesizer(
                m_master_circuit_processor,
                m_polyphonic_circuit_processor,
                GAMMOU_STANDALONE_INPUT_COUNT,
                GAMMOU_STANDALONE_OUTPUT_COUNT,
                GAMMOU_STANDALONE_CHANNEL_COUNT,
                GAMMOU_STANDALONE_PARAMETER_COUNT)
        {
        }

        application::~application()
        {
            stop_audio();
        }

        void application::run()
        {
            setting_widget setting{*this};

            Gui::synthesizer_gui main_gui{
                &m_synthesizer,
                &m_synthesizer_mutex,
                *this};

            View::application_display setting_display{setting};
            View::application_display main_gui_display{main_gui};

            setting_display.open("Audio Devices");
            main_gui_display.open("Gammou");

            main_gui_display.wait();
            setting_display.close();
            setting_display.wait();
        }

        double application::get_parameter_value(
            const unsigned int index)
        {
            //  TODO
            return m_synthesizer.get_parameter_value(index);
        }

        void application::set_parameter_value(
            const unsigned int index, const double value)
        {
            //  TODO
        }

        unsigned int application::get_parameter_count()
        {
            return GAMMOU_STANDALONE_PARAMETER_COUNT;
        }

        void application::start_audio(
            const RtAudio::Api api,
            const unsigned int device_index,
            const unsigned int sample_rate)
        {
            stop_audio();

            RtAudio::StreamParameters param;
            unsigned int buffer_size = 
                AUDIO_BUFFER_SIZE;

            param.deviceId = device_index;
            param.nChannels = 2;
            param.firstChannel = 0;

            try {
                m_audio =
                    std::make_unique<RtAudio>(api);

                m_audio->openStream(
                    &param, nullptr,
                    RTAUDIO_FLOAT64,
                    sample_rate,
                    &buffer_size,
                    snd_callback,
                    this);

                m_synthesizer.set_sample_rate(
                    static_cast<double>(sample_rate));
                m_audio->startStream();
            }
            catch(...) {}
        }

        void application::stop_audio()
        {
            if (m_audio) {
                try {
                    m_audio->stopStream();
                }
                catch(...) {}
            }
        }

        int application::snd_callback(
            void *output_buffer,
            void *input_buffer,
            unsigned int sample_count,
            double streamTime,
            RtAudioStreamStatus status,
            void *user_data)
        {
            application& self = 
                *(reinterpret_cast<application*>(user_data));

            double input[2] = {0.0, 0.0}; // a stubb
            double *output = (double *)output_buffer;

            self.m_synthesizer_mutex.lock();
            for (unsigned int i = 0; i < sample_count; ++i, output += 2)
                self.m_synthesizer.process(input, output);
            self.m_synthesizer_mutex.unlock();

            return 0;
        }

    } /* Standalone */

} /* Gammou */