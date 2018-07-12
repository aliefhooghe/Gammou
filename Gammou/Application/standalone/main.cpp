
#ifdef OSSIA_BINDING // first !!
#include <ossia/network/oscquery/oscquery_server.hpp>
#include <ossia/network/local/local.hpp>
#include "ossia_binding/synthesizer_protocol.h"
#endif

#include <mutex>
#include <synthesizer.h>
#include <synthesizer_gui.h>

#include "jit_frame_processor/jit_frame_processor.h"

#include "../debug.h"

#include <RtAudio.h>


struct snd_callback_data{
    Gammou::Sound::synthesizer *synthesizer;
    std::mutex *synthesizer_mutex;
};

int snd_callback(void *output_buffer, void *input_buffer, unsigned int sample_count, 
                    double streamTime, RtAudioStreamStatus status, void *void_data)
{
    double input[2] = {-1.0, 1.0};
    double *output = (double*)output_buffer;
    snd_callback_data *data = (snd_callback_data*)void_data;

    data->synthesizer_mutex->lock();
    for(unsigned int i = 0; i < sample_count; ++i, output += 2){
       data->synthesizer->process(input, output);
    }

    data->synthesizer_mutex->unlock();

    return 0;
}

int main()
{
   RtAudio dac;

   if ( dac.getDeviceCount() < 1 ) {
        std::cerr << "No audio devices found!\n";
        exit(0);
    }

    RtAudio::StreamParameters parameters;
    parameters.deviceId = dac.getDefaultOutputDevice();
    parameters.nChannels = 2;
    parameters.firstChannel = 0;
    unsigned int sampleRate = 48000;
    unsigned int bufferFrames = 256; // 256 sample frames

    std::mutex synthesizer_mutex;

    Gammou::Sound::jit_frame_processor processor1;
    Gammou::Sound::jit_frame_processor processor2;

    //Gammou::Process::bytecode_frame_processor<double> processor1;
   // Gammou::Process::bytecode_frame_processor<double> processor2;

    Gammou::Sound::synthesizer synthesizer(
        processor1, processor2, 
        2, 2, 128, 16);

    synthesizer.set_sample_rate(48000);

	Gammou::Gui::synthesizer_gui window(&synthesizer, &synthesizer_mutex);
    Gammou::View::application_display display(window);

    //   Start Rt Audio 

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
    
    //  Start OscQuery

#ifdef OSSIA_BINDING
    auto protocol = std::make_unique<ossia::net::multiplex_protocol>();
    auto& multiplex = *protocol;

    std::cout << "Starting OSC Query server" << std::endl;

    ossia::net::generic_device gammou_ossia{std::move(protocol), "Gammou"};
	multiplex.expose_to(std::make_unique<ossia::oscquery::oscquery_server_protocol>(1234, 5678));
	multiplex.expose_to(std::make_unique<Gammou::synthesizer_ossia_protocol>(&synthesizer));
#endif

    //-----
    display.open("Gammou");
    display.wait();

    //------

    DEBUG_PRINT("Closing Sound Stream\n");

    try {
        // Stop the stream
        dac.stopStream();
    }
    catch (RtAudioError& e) {
        e.printMessage();
    }
    
    if ( dac.isStreamOpen() ) 
        dac.closeStream();
    
    return 0;
}
