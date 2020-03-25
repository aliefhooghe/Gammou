#include <algorithm>
#include "synthesizer.h"

namespace Gammou {

    synthesizer::synthesizer(
            llvm::LLVMContext &llvm_context,
            unsigned int input_count,
            unsigned int output_count,
            unsigned int voice_count)
    :   _llvm_context{llvm_context},
        _input_count{input_count},
        _output_count{output_count},
        _midi_input_values(0.f, voice_count * midi_input_count),
        _master_circuit_context{_llvm_context, 1u},
        _polyphonic_circuit_context{_llvm_context, voice_count},
        _from_polyphonic{0u, polyphonic_to_master_channel_count},
        _output{output_count, 0u},
        _midi_input{0u, midi_input_count},
        _to_master{polyphonic_to_master_channel_count, 0u},
        _voice_manager{voice_count},
        _voice_lifetime(0u, voice_count)
    {
    }

    void synthesizer::process_sample(const float input[], float output[]) noexcept
    {
        _process_one_sample(input, output);

        for (auto i = 0u; i < _output_count; ++i)
            output[i] *= _volume;
    }

    void synthesizer::process_buffer(std::size_t sample_count, const float[],float outputs[]) noexcept
    {
        for (auto i = 0u; i < sample_count; ++i)
            _process_one_sample(nullptr, outputs + i * _output_count);

        const auto buffer_size = _output_count * sample_count;
        for (auto i = 0u; i < buffer_size; ++i)
            outputs[i] *= _volume;
    }

    void synthesizer::midi_note_on(uint8_t note, float velocity)
    {
        voice_manager::voice voice;
        if (_voice_manager.note_on(note, voice)) {
            auto midi_input = get_voice_midi_input(voice);
            midi_input[gate] = 1.f;
            midi_input[pitch] = 444.; /** @todo **/
            _voice_lifetime[voice] = _voice_disappearance_sample_count;
        }
    }

    void synthesizer::midi_note_off(uint8_t note, float velocity)
    {
        voice_manager::voice voice;
        if (_voice_manager.note_off(note, voice)) {
            auto midi_input = get_voice_midi_input(voice);
            midi_input[gate] = 0.f;
        }
    }

    void synthesizer::midi_control_change(uint8_t , float )
    {
        /** @todo **/
    }


    void synthesizer::compile_master_circuit()
    {
        _master_circuit_context.compile({_from_polyphonic}, {_output});
    }

    void synthesizer::compile_polyphonic_circuit()
    {
        _polyphonic_circuit_context.compile({_midi_input}, {_to_master});
    }

    void synthesizer::_process_one_sample(const float[], float output[]) noexcept
    {
        float polyphonic_output[polyphonic_to_master_channel_count] = {0.f};

        //  Sum the outputs of each active voice
        _voice_manager.foreach_active_voice(
            [&polyphonic_output, this](const auto voice) -> bool
            {
                float out_tmp[polyphonic_to_master_channel_count];

                //  process this voice
                _polyphonic_circuit_context.process(
                    voice,
                    get_voice_midi_input(voice),
                    out_tmp);

                //  add
                for (auto i = 0u; i < polyphonic_to_master_channel_count; ++i)
                    polyphonic_output[i] += out_tmp[i];

                //  voice disappearance detection
                auto max_voice_value =
                    *std::max_element(out_tmp, out_tmp + polyphonic_to_master_channel_count);

                if (max_voice_value <= voice_disappearance_treshold) {
                    if (0u == --_voice_lifetime[voice])
                        return false;
                }
                else {
                    _voice_lifetime[voice] = _voice_disappearance_sample_count;
                }

                return true;
            }
        );

        //  Apply master processing
        _master_circuit_context.process(polyphonic_output, output);
    }
}