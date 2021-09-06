#ifndef GAMMOU_VOICE_MODE_SELECTOR_H
#define GAMMOU_VOICE_MODE_SELECTOR_H

#include <view.h>
#include "synthesizer/synthesizer.h"

namespace Gammou {

    using voicing_mode_model =
            View::storage_directory_model<
                std::string,
                synthesizer::voice_mode,
                View::alphabetical_compare>;

    class voice_mode_selector : public View::owning_directory_view<voicing_mode_model>
    {

    public:
        voice_mode_selector(synthesizer&);
        ~voice_mode_selector() override = default;

        void sync();

    private:
        void _select_voice_mode(synthesizer::voice_mode mode);

        synthesizer& _synthesizer;
    };

}

#endif