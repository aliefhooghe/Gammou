#include <mutex>

#include <view.h>
#include <synthesizer.h>
#include <synthesizer_gui.h>




int main(){
    std::mutex synthesizer_mutex;
    Gammou::Sound::synthesizer synthesizer(2, 2, GAMMOU_SYNTHESIZER_CHANNEL_COUNT, 16);
	Gammou::Gui::synthesizer_gui window(&synthesizer, &synthesizer_mutex);

    while(1){
        ;
    }

    return 0;
}
