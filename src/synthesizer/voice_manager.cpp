
#include <DSPJIT/log.h>

#include "voice_manager.h"

namespace Gammou
{
    /** Map midi note number to Hz frequencies **/
    static constexpr float note_frequencies[128] =
    {
        8.176f,8.662f,9.177f,9.723f,10.301f,10.913f,11.562f,12.250f,12.978f,13.750f,14.568f,15.434f,16.352f,17.324f,18.354f,19.445f,
        20.602f,21.827f,23.125f,24.500f,25.957f,27.500f,29.135f,30.868f,32.703f,34.648f,36.708f,38.891f,41.203f,43.654f,46.249f,48.999f,
        51.913f,55.000f,58.270f,61.735f,65.406f,69.296f,73.416f,77.782f,82.407f,87.307f,92.499f,97.999f,103.826f,110.000f,116.541f,123.471f,
        130.813f,138.591f,146.832f,155.563f,164.814f,174.614f,184.997f,195.998f,207.652f,220.000f,233.082f,246.942f,261.626f,277.183f,293.665f,311.127f,
        329.628f,349.228f,369.994f,391.995f,415.305f,440.000f,466.164f,493.883f,523.251f,554.365f,587.330f,622.254f,659.255f,698.456f,739.989f,783.991f,
        830.609f,880.000f,932.328f,987.767f,1046.502f,1108.731f,1174.659f,1244.508f,1318.510f,1396.913f,1479.978f,1567.982f,1661.219f,1760.000f,1864.655f,1975.533f,
        2093.005f,2217.461f,2349.318f,2489.016f,2637.020f,2793.826f,2959.955f,3135.963f,3322.438f,3520.000f,3729.310f,3951.066f,4186.009f,4434.922f,4698.636f,4978.032f,
        5274.041f,5587.652f,5919.911f,6271.927f,6644.875f,7040.000f,7458.620f,7902.133f,8372.018f,8869.844f,9397.273f,9956.063f,10548.080f,11175.300f,11839.820f,12543.850f
    };

    voice_manager::voice_manager(
        std::size_t voice_count,
        DSPJIT::graph_execution_context& polyphonic_context)
    :   _voice_lifetime(voice_count, 0u),
        _midi_input_values(voice_count * midi_input_count, 0.f),
        _polyphonic_context{polyphonic_context}
    {
        _voices.reserve(voice_count);
        for (auto i = 0u; i < voice_count; ++i)
            _voices.emplace_back(0u, i);
        _on_voice_end = _voices.begin();
        _active_voices_end = _voices.begin();
    }


    void voice_manager::set_voice_mode(mode m)
    {
        _mode = m;
    }

    voice_manager::mode voice_manager::get_voice_mode() const noexcept
    {
        return _mode;
    }

    bool voice_manager::note_on(note n, float velocity)
    {
        if (_mode == mode::LEGATO) {
            auto it = _find_on_voice();
            if (it != _on_voice_end) {
                it->first = n;
                _setup_voice(it->second, n, velocity);
                return true;
            }
            // else : no voice are on : allocate a voice
        }

        if (_allocate_voice(n, velocity))
            return true;
        else
            return false;
    }

    bool voice_manager::note_off(note n)
    {
        auto it = _find_note_on_voice(n);
        if (it != _on_voice_end) {
            auto midi_input = _get_voice_midi_input(it->second);
            midi_input[gate] = 0.f;
            _voice_off(it);
            return true;
        }
        else {
            return false;
        }
    }

    void voice_manager::process_one_sample(float output[])
    {
        for (auto i = 0u; i < polyphonic_to_master_channel_count; ++i)
            output[i] = 0.f;

        for (auto it = _voices.begin(); it != _active_voices_end;) {
            const auto voice = it->second;

            // Compute voice output
            float out_tmp[polyphonic_to_master_channel_count];

            //  process this voice
            _polyphonic_context.process(
                voice,
                _get_voice_midi_input(it->second),
                out_tmp);

            //  add
            for (auto i = 0u; i < polyphonic_to_master_channel_count; ++i)
                output[i] += out_tmp[i];

            //  voice disappearance detection
            auto max_voice_value =
                *std::max_element(
                    out_tmp, out_tmp + polyphonic_to_master_channel_count,
                    [](const auto& s1, const auto& s2) { return std::abs(s1) < std::abs(s2); });

            if (std::abs(max_voice_value) <= _voice_disappearance_treshold) {
                if (0u == --_voice_lifetime[voice]) {
                    LOG_DEBUG("[synthesizer][process one sample] Shut down voice %u\n", voice);
                    if (it < _on_voice_end)
                        _voice_off(it);
                    _voice_stop(it);
                    continue;
                }
            }
            else {
                _voice_lifetime[voice] = _voice_disappearance_sample_count;
            }

            ++it;
        }
    }

    bool voice_manager::_allocate_voice(note n, float velocity)
    {
        //  if there is a free voice
        if (_active_voices_end != _voices.end()) {
            const auto free_it = _active_voices_end;
            const auto voice = free_it->second;

            // Initialize the voice when it is not active
            _setup_voice(voice, n, velocity);
            _polyphonic_context.initialize_state(voice);

            _active_voices_end++;
            auto allocated_place_it = _on_voice_end++;
            std::iter_swap(free_it, allocated_place_it);
            allocated_place_it->first = n;
            return true;
        }
        else {
            return false;
        }
    }

    void voice_manager::_setup_voice(voice v, note n, float velocity)
    {
        auto midi_input = _get_voice_midi_input(v);
        midi_input[gate] = 1.f;
        midi_input[pitch] = note_frequencies[n];
        midi_input[attack] = velocity;
        _voice_lifetime[v] = _voice_disappearance_sample_count;
    }

    void voice_manager::_voice_off(voice_store::iterator it)
    {
        std::iter_swap(it, --_on_voice_end);
    }

    void voice_manager::_voice_stop(voice_store::iterator it)
    {
        std::iter_swap(it, --_active_voices_end);
    }

    voice_manager::voice_store::iterator voice_manager::_find_note_on_voice(note n)
    {
        for (auto it = _voices.begin(); it != _on_voice_end; ++it) {
            if (it->first == n)
                return it;
        }
        return _on_voice_end;
    }

    voice_manager::voice_store::iterator voice_manager::_find_on_voice()
    {
        if (_on_voice_end != _voices.begin())
            return std::prev(_on_voice_end);
        else
            return _on_voice_end;
    }
}