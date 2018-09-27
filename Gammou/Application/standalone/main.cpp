
#ifdef OSSIA_BINDING // first !!
#include <ossia/network/oscquery/oscquery_server.hpp>
#include <ossia/network/local/local.hpp>
#include "ossia_binding/synthesizer_protocol.h"
#endif

#include <mutex>
#include <synthesizer.h>
#include <midi_driver/midi_driver.h>
#include <synthesizer_gui.h>

#include "jit_frame_processor/jit_frame_processor.h"

#include "../debug.h"

#include <RtAudio.h>
#include <RtMidi.h>

#define SAMPLE_RATE 44100

struct snd_callback_data {
    Gammou::Sound::synthesizer *synthesizer;
    std::mutex *synthesizer_mutex;
};


int snd_callback(
    void *output_buffer, 
    void *input_buffer, 
    unsigned int sample_count, 
    double streamTime, 
    RtAudioStreamStatus status, 
    void *user_data)
{
    double *input = (double*)input_buffer;
    double *output = (double*)output_buffer;
    snd_callback_data *data = (snd_callback_data*)user_data;

    data->synthesizer_mutex->lock();
    for(unsigned int i = 0; i < sample_count; ++i, output += 2, input += 2)
        data->synthesizer->process(input, output);

    data->synthesizer_mutex->unlock();

    return 0;
}

void midi_callback(
    double deltatime, 
    std::vector<unsigned char> *message, 
    void *user_data)
{
    Gammou::Sound::midi_driver *driver = 
        reinterpret_cast<Gammou::Sound::midi_driver*>(user_data);
    driver->handle_midi_event(*message);
}

int main()
{

    //  RT AUDio
   RtAudio dac;

   if ( dac.getDeviceCount() < 1 ) {
        std::cerr << "No audio devices found!\n";
        exit(0);
    }

    RtAudio::StreamParameters input_param, output_param;

    input_param.deviceId = dac.getDefaultInputDevice();
    input_param.nChannels = 2;
    input_param.firstChannel = 0;

    output_param.deviceId = dac.getDefaultOutputDevice();
    output_param.nChannels = 2;
    output_param.firstChannel = 0;
    
    const unsigned int sampleRate = SAMPLE_RATE;
    unsigned int bufferFrames = 512; 

    // Gammou

    std::mutex synthesizer_mutex;

    //Gammou::Sound::jit_frame_processor processor1;
    //Gammou::Sound::jit_frame_processor processor2;

    Gammou::Process::bytecode_frame_processor<double> processor1;
    Gammou::Process::bytecode_frame_processor<double> processor2;

    Gammou::Sound::synthesizer synthesizer(
        processor1, processor2, 
        2, 2, 128, 16);

    synthesizer.set_sample_rate(SAMPLE_RATE);

    //  Midi Driver
    Gammou::Sound::midi_driver midi_driver(synthesizer);

	//  Grapihc User Interface

    Gammou::Gui::synthesizer_gui window(&synthesizer, &synthesizer_mutex);
    Gammou::View::application_display display(window);

    //  RtMidi

    RtMidiIn midi_input;
    const unsigned int midi_input_port_count = 
        midi_input.getPortCount();

    DEBUG_PRINT("%u Midi Port Where Found\n", midi_input_port_count);

    if (midi_input_port_count <= 1) {
        DEBUG_PRINT("No Midi Port Where Found\n");
    }
    else {
        midi_input.openPort(1);
        midi_input.setCallback(midi_callback, &midi_driver);
    }

    //   Start Rt Audio 

    struct snd_callback_data data = {&synthesizer, &synthesizer_mutex};

    try {
        dac.openStream( 
            &output_param,
            &input_param, 
            RTAUDIO_FLOAT64,
        	sampleRate, 
            &bufferFrames, 
            &snd_callback, 
            (void *)(&data));

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
    

    DEBUG_PRINT("FIN\n");
    return 0;
}
