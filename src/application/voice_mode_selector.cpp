#include "voice_mode_selector.h"

namespace Gammou {

    voice_mode_selector::voice_mode_selector(synthesizer& synth)
    :   View::owning_directory_view<voicing_mode_model>(
            std::make_unique<voicing_mode_model>(),
            140, 90, 16, 14),
        _synthesizer{synth}
    {
        auto& model = data_model();
        model.insert_value("Polyphonic", synthesizer::voice_mode::POLYPHONIC);
        // model.insert_value("Monophonic", synthesizer::voice_mode::MONOPHONIC);
        model.insert_value("Legato", synthesizer::voice_mode::LEGATO);

        set_value_select_callback(
            [this](const auto& mode) { _synthesizer.set_voice_mode(mode); });

        sync();
    }

    void voice_mode_selector::sync()
    {
        _select_voice_mode(_synthesizer.get_voice_mode());
    }

    void voice_mode_selector::_select_voice_mode(synthesizer::voice_mode mode)
    {
        select_item_if(
            [mode](const auto& item)
            {
                if (std::holds_alternative<synthesizer::voice_mode>(item))
                    return mode == std::get<synthesizer::voice_mode>(item);
                else
                    return false;
            });
    }

}