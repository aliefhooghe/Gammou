#include <algorithm>
#include <llvm/Transforms/Utils/Cloning.h>

#include "synthesizer.h"

#include <iostream>
namespace Gammou {

    /**
     *  \brief Map midi note number to Hz frequencies
     */
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

    synthesizer::synthesizer(
            llvm::LLVMContext &llvm_context,
            unsigned int input_count,
            unsigned int output_count,
            unsigned int voice_count,
            const opt_level level,
            const llvm::TargetOptions& options)
    :   _llvm_context{llvm_context},
        _input_count{input_count},
        _output_count{output_count},
        _midi_input_values(voice_count * midi_input_count, 0.f),
        _master_circuit_context{_llvm_context, 1u, level, options},
        _polyphonic_circuit_context{_llvm_context, voice_count, level, options},
        _from_polyphonic{0u, polyphonic_to_master_channel_count},
        _output{output_count, 0u},
        _midi_input{0u, midi_input_count},
        _to_master{polyphonic_to_master_channel_count, 0u},
        _voice_manager{voice_count},
        _voice_lifetime(voice_count, 0u)
    {
        std::fill_n(_midi_learn_map.begin(), 255u, parameter_count);
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

        auto initialize_voice = [this, note, velocity](const auto& voice){
            auto midi_input = get_voice_midi_input(voice);
            midi_input[gate] = 1.f;
            midi_input[pitch] = note_frequencies[note];
            midi_input[attack] = velocity;
            _voice_lifetime[voice] = _voice_disappearance_sample_count;
            _polyphonic_circuit_context.initialize_state(voice);
        };

        if (!_voice_manager.note_on(note, initialize_voice)) {
            LOG_ERROR("[synthesizer][not on] No anymore free voice\n");
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

    void synthesizer::midi_control_change(uint8_t control, float value)
    {
        if (_midi_learning) {
            _midi_learn_map[control] = _learning_param;
            _midi_learning = false;
            LOG_DEBUG("[synthesizer][midi learn] Midi control %u assigned to parameter %u\n", control, _learning_param);
        }

        auto param_id = _midi_learn_map[control];
        if (param_id != parameter_count)
            _parameter_manager.set_parameter_nomalized(param_id, value);
    }

    synthesizer::parameter synthesizer::allocate_parameter(float normalized_initial_value)
    {
        auto param = _parameter_manager.allocate_parameter(normalized_initial_value);
        midi_disassign(param);
        return param;
    }

    void synthesizer::midi_learn(const parameter& param)
    {
        LOG_DEBUG("[synthesizer][midi learn] start midi learn for parameter %u\n", param.id());
        midi_disassign(param);
        _learning_param = param.id();
        _midi_learning = true;
    }

    void synthesizer::midi_disassign(const parameter& param)
    {
        if (_midi_learning && _learning_param == param.id())
            _midi_learning = false;

        for (auto& learned_param : _midi_learn_map) {
            if (learned_param == param.id())
                learned_param = parameter_count;
        }
    }

    void synthesizer::midi_assign_control(uint8_t control, const parameter& param)
    {
        _midi_learn_map[control] = param.id();
    }

    bool synthesizer::midi_assigned_to_control(uint8_t& control, const parameter& param)
    {
        for (auto i = 0u; i < 256; ++i) {
            if (_midi_learn_map[i] == param.id()) {
                control = i;
                return true;
            }
        }
        return false;
    }

    void synthesizer::add_module(std::unique_ptr<llvm::Module>&& m)
    {
        _master_circuit_context.add_module(llvm::CloneModule(*m));
        _polyphonic_circuit_context.add_module(std::move(m));
    }

    void synthesizer::compile_master_circuit()
    {
        _master_circuit_context.compile({_from_polyphonic}, {_output});
    }

    void synthesizer::compile_polyphonic_circuit()
    {
        _polyphonic_circuit_context.compile({_midi_input}, {_to_master});
    }

    bool synthesizer::update_program() noexcept
    {
        const auto b1 = _master_circuit_context.update_program();
        const auto b2 = _polyphonic_circuit_context.update_program();
        return b1 || b2; // use var in order to avoid lazy evaluation side efects
    }

    void synthesizer::_process_one_sample(const float[], float output[]) noexcept
    {
        float polyphonic_output[polyphonic_to_master_channel_count] = {0.f};

        //  Update parameters
        _parameter_manager.process_one_sample();

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
                    *std::max_element(
                        out_tmp, out_tmp + polyphonic_to_master_channel_count,
                        [](const auto& s1, const auto& s2) { return std::abs(s1) < std::abs(s2); });

                if (std::abs(max_voice_value) <= voice_disappearance_treshold) {
                    if (0u == --_voice_lifetime[voice]) {
                        LOG_DEBUG("[synthesizer][process one sample] Shut down voice %u\n", voice);
                        return false;
                    }
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