
#include <mutex>
#include <synthesizer.h>
#include <synthesizer_gui.h>

#include <unistd.h>
#include "../debug.h"

#include <RtAudio.h>


struct snd_callback_data{
    Gammou::Sound::synthesizer *synthesizer;
    std::mutex *synthesizer_mutex;
};

int snd_callback(void *output_buffer, void *input_buffer, unsigned int sample_count, 
                    double streamTime, RtAudioStreamStatus status, void *void_data)
{
    double *input = (double*)input_buffer;
    double *output = (double*)output_buffer;
    snd_callback_data *data = (snd_callback_data*)void_data;

   data->synthesizer_mutex->lock();


    for(unsigned int i = 0; i < sample_count; ++i, output += 2, input += 2)
       data->synthesizer->process(input, output);

    data->synthesizer_mutex->unlock();

    return 0;
}

int main()
{
   RtAudio dac;

   if ( dac.getDeviceCount() < 1 ) {
        std::cerr << "\nNo audio devices found!\n";
        exit( 0 );
    }

    RtAudio::StreamParameters parameters;
    parameters.deviceId = dac.getDefaultOutputDevice();
    parameters.nChannels = 2;
    parameters.firstChannel = 0;
    unsigned int sampleRate = 44100;
    unsigned int bufferFrames = 256; // 256 sample frames

    std::mutex synthesizer_mutex;
    Gammou::Sound::synthesizer synthesizer(2, 2, 128, 16);
	Gammou::Gui::synthesizer_gui window(&synthesizer, &synthesizer_mutex);

    struct snd_callback_data data = {&synthesizer, &synthesizer_mutex};

    try {
        dac.openStream( &parameters, nullptr, RTAUDIO_FLOAT64,
        		sampleRate, &bufferFrames, &snd_callback, (void *)(&data));

        dac.startStream();
    }
    catch ( RtAudioError& e ) {
        e.printMessage();
        exit( 0 );
    }
    
    DEBUG_PRINT("Entering Sleep\n");
    
    while(1){
        // Avoid burning cpu
        sleep(100);
    }
    
    try {
        // Stop the stream
        dac.stopStream();
    }
    catch (RtAudioError& e) {
        e.printMessage();
    }
    if ( dac.isStreamOpen() ) dac.closeStream();

    return 0;
}
