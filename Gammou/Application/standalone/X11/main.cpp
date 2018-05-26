
#include <mutex>
#include <synthesizer.h>
#include <synthesizer_gui.h>
#include <bytecode_frame_processor.h>
#include <jit_frame_processor/jit_frame_processor.h>

#include <unistd.h>
#include <sys/time.h>
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


double get_time()
{
	struct timeval tps;
	gettimeofday(&tps, NULL);
	return (double)tps.tv_sec + 0.000001 * (double)tps.tv_usec;
}


void test_syn(Gammou::Sound::synthesizer& syn, const unsigned int k)
{
    double input[2] = {0.0, 0.0};
    double output[2] = {0.0, 0.0};

    for(unsigned int i = 0; i < k; ++i)
        syn.process(input, output);
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

    Gammou::Sound::jit_frame_processor processor;
    Gammou::Sound::synthesizer synthesizer(processor, 2, 2, 128, 16);
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
    
    
/*



    const unsigned int count = 32;

    Gammou::Process::bytecode_frame_processor<double> bytecode_processor;
    Gammou::Sound::synthesizer synthesizerb(bytecode_processor, 2, 2, 128, 16);

    Gammou::Sound::Builtin::sin_component *prevb = new Gammou::Sound::Builtin::sin_component(128);
    synthesizerb.add_sound_component_on_master_circuit(prevb);

    for(unsigned int i = 0; i < count; ++i) {
        Gammou::Sound::Builtin::sin_component *nextb = new Gammou::Sound::Builtin::sin_component(128);
        synthesizerb.add_sound_component_on_master_circuit(nextb);
        prevb->connect_to(0, nextb, 0);
        prevb = nextb;
    }

    prevb->connect_to(0, synthesizerb.get_master_main_output(), 0);




    Gammou::Sound::jit_frame_processor jit_processor;
    Gammou::Sound::synthesizer synthesizerj(jit_processor, 2, 2, 128, 16);

    Gammou::Sound::Builtin::sin_component *prevj = new Gammou::Sound::Builtin::sin_component(128);
    synthesizerj.add_sound_component_on_master_circuit(prevj);

    for(unsigned int i = 0; i < count; ++i) {
        Gammou::Sound::Builtin::sin_component *nextj = new Gammou::Sound::Builtin::sin_component(128);
        synthesizerj.add_sound_component_on_master_circuit(nextj);
        prevj->connect_to(0, nextj, 0);
        prevj = nextj;
    }

    prevj->connect_to(0, synthesizerj.get_master_main_output(), 0);


    printf("############################################################################\n\n\nStarting test :\n\n");

   for (unsigned int i = 1; i < 5; ++i) {
       const unsigned int k = i * 44100 * 60;
       double t1, t2;
       double bytecode_duration, jit_duration;

        t1 = get_time();
        test_syn(synthesizerb, k);
        t2 = get_time();

        bytecode_duration = t2 - t1;

        t1 = get_time();
        test_syn(synthesizerj, k);
        t2 = get_time();

        jit_duration = t2 - t1;
        
        printf("%u minutes : bytecode_duration = %lf, jit_duration = %lf, ratio = %lf\n", 
            i, bytecode_duration, jit_duration, bytecode_duration / jit_duration);
   }
*/
    
    
    return 0;
}
