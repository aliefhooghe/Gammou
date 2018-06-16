
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


//#define TEST_PERFORMANCE

int main()
{

#ifndef TEST_PERFORMANCE

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

    Gammou::Sound::jit_frame_processor jit_processor1;
    Gammou::Sound::jit_frame_processor jit_processor2;
    Gammou::Sound::synthesizer synthesizer(
        jit_processor1, jit_processor2, 2, 2, 128, 16);
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
    
    DEBUG_PRINT("Opening Window\n");
    
    
    window.open();


    while(getchar() != 'q'){
        printf("Nop\n");
    }

    window.close();

    try {
        // Stop the stream
        dac.stopStream();
    }
    catch (RtAudioError& e) {
        e.printMessage();
    }
    if ( dac.isStreamOpen() ) dac.closeStream();
    
    
#else
    const unsigned int component_count = 1;
    const unsigned int test_count = 64;

    double bytecode_time_sum = 0.0;
    double jit_time_sum = 0.0;
    double ratio_sum = 0.0;

    Gammou::Process::bytecode_frame_processor<double> bytecode_processor1;
    Gammou::Process::bytecode_frame_processor<double> bytecode_processor2;
    Gammou::Sound::synthesizer synthesizerb(
        bytecode_processor1, bytecode_processor2, 2, 2, 128, 16);

    Gammou::Sound::Builtin::sin_component *prevb = new Gammou::Sound::Builtin::sin_component(128);

    synthesizerb.add_sound_component_on_polyphonic_circuit(prevb);
    synthesizerb.get_polyphonic_circuit_midi_input()->connect_to(1, prevb, 0);

    for(unsigned int i = 0; i < component_count; ++i) {
        Gammou::Sound::Builtin::sin_component *nextb = new Gammou::Sound::Builtin::sin_component(128);
        synthesizerb.add_sound_component_on_polyphonic_circuit(nextb);
        prevb->connect_to(0, nextb, 0);
        prevb->connect_to(0, nextb, 1);
        prevb = nextb;
    }

    prevb->connect_to(0, synthesizerb.get_polyphonic_circuit_master_input(), 0);
    synthesizerb.get_master_circuit_polyphonic_output()->connect_to(0, synthesizerb.get_master_main_output(), 0);

    for(unsigned int i = 0; i < 16; ++i)
        synthesizerb.send_note_on(20 + i, 0.5);

/*
    Gammou::Process::bytecode_frame_processor<double> bytecode_processor1;
    Gammou::Process::bytecode_frame_processor<double> bytecode_processor2;
    Gammou::Sound::synthesizer synthesizerb(
        bytecode_processor1, bytecode_processor2, 2, 2, 128, 16);

    Gammou::Sound::Builtin::sin_component *prevb = new Gammou::Sound::Builtin::sin_component(128);

    synthesizerb.add_sound_component_on_polyphonic_circuit(prevb);
    synthesizerb.get_polyphonic_circuit_midi_input().connect_to(1, prevb, 0);

    for(unsigned int i = 0; i < component_count; ++i) {
        Gammou::Sound::Builtin::sin_component *nextb = new Gammou::Sound::Builtin::sin_component(128);
        synthesizerb.add_sound_component_on_polyphonic_circuit(nextb);
        prevb->connect_to(0, nextb, 0);
        prevb->connect_to(0, nextb, 1);
        prevb = nextb;
    }

    prevb->connect_to(0, synthesizerb.get_polyphonic_circuit_master_input(), 0);
    synthesizer.get_master_circuit_polyphonic_output().connect_to(synthesizerb.get_master_main_output());
*/

    printf("############################################################################\n\n\nStarting test :\n\n");

   for (unsigned int i = 0; i < test_count; ++i) {
       const unsigned int k = 2 * 44100 * 60;
       double t1, t2;
       double bytecode_duration, jit_duration;

        t1 = get_time();
        test_syn(synthesizerb, k);
        t2 = get_time();

        bytecode_duration = t2 - t1;

/*
        t1 = get_time();
        test_syn(synthesizerj, k);
        t2 = get_time();

        jit_duration = t2 - t1;
        
        const double ratio = bytecode_duration / jit_duration;
*/
 //       ratio_sum += ratio;
        bytecode_time_sum += bytecode_duration;
 //       jit_time_sum += jit_duration;
        
        printf("bytecode_duration = %lf\n"/*, jit_duration = %lf, ratio = %lf\n"*/, 
            bytecode_duration/*, jit_duration, ratio*/);
   }
    
    const double ratio_mean = ratio_sum / (double) test_count;
    //printf("\n\nratio moyen : %lf soit %lf %% de gain\n", ratio_mean, 100.0 * (ratio_mean - 1.0));
    printf("\nDurées moyennes : \n\t\t\t\t- bytecode \t\t: %lf\n\n\n", bytecode_time_sum / (double) test_count);
    //printf("############################################################################\n");
    


#endif

    return 0;
}
