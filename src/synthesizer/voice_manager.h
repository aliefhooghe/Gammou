#ifndef GAMMOU_VOICE_MANAGER_H_
#define GAMMOU_VOICE_MANAGER_H_

#include <numeric>
#include <unordered_map>
#include <vector>

#include <DSPJIT/graph_execution_context.h>

namespace Gammou
{
    class voice_manager
    {
    public:
        static constexpr auto voice_disappearance_treshold_default = 0.0003f;
        static constexpr auto polyphonic_to_master_channel_count = 2u;

        /**
         *  \enum midi_inputs The available channel midi inputs
         */
        enum midi_inputs {
            gate = 0u,          /** 1. when note is on, 0. otherwise **/
            pitch,              /** the pitch of the note being played (Hz) **/
            attack,             /** the attack velocity in [0., 1.] **/

            midi_input_count
        };

        enum class mode
        {
            POLYPHONIC,
            LEGATO
        };

        using note = uint8_t;
        using voice = uint32_t;
        using voice_store = std::vector<std::pair<note, voice>>;

        voice_manager(
            std::size_t voice_count,
            DSPJIT::graph_execution_context& polyphonic_context);

        void set_voice_mode(mode);
        voice_manager::mode get_voice_mode() const noexcept;
        bool note_on(note, float velocity);
        bool note_off(note);
        void process_one_sample(float output[]);

    private:
        bool _allocate_voice(note n, float velocity);
        void _setup_voice(voice, note, float velocity);
        void _voice_off(voice_store::iterator it);
        void _voice_stop(voice_store::iterator it);
        voice_store::iterator _find_note_on_voice(note n);
        voice_store::iterator _find_on_voice();

        inline auto _get_voice_midi_input(voice voice) noexcept
        {
            return _midi_input_values.data() +
                static_cast<int>(voice) * static_cast<int>(midi_input_count);
        }

        //  Midi input values are stored here for every running voices
        std::vector<float> _midi_input_values;

        unsigned int _voice_disappearance_sample_count{20000u};
        float _voice_disappearance_treshold{voice_disappearance_treshold_default};
        std::vector<unsigned int> _voice_lifetime;

        voice_store _voices{};
        voice_store::iterator _on_voice_end;
        voice_store::iterator _active_voices_end;
        mode _mode{mode::POLYPHONIC};

        DSPJIT::graph_execution_context& _polyphonic_context;
    };
} // namespace Gammou


#endif