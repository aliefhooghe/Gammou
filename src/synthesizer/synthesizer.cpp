#include <algorithm>
#include <llvm/Transforms/Utils/Cloning.h>

#include "synthesizer.h"

#include <iostream>
#include <fstream>

namespace Gammou {

    /**
     *  Circuit controllers implementation
     */
    synthesizer::master_circuit_controller::master_circuit_controller(synthesizer& synth)
    :   _synthesizer{synth}
    {
    }

    void synthesizer::master_circuit_controller::compile()
    {
        LOG_INFO("[synthesizer] Compile master circuit\n");
        _synthesizer._master_circuit_context.compile({_synthesizer._from_polyphonic}, {_synthesizer._output});
    }

    void synthesizer::master_circuit_controller::register_static_memory_chunk(const DSPJIT::compile_node_class &node, std::vector<uint8_t> &&data)
    {
        _synthesizer._master_circuit_context.register_static_memory_chunk(node, std::move(data));
    }

    void synthesizer::master_circuit_controller::free_static_memory_chunk(const DSPJIT::compile_node_class &node)
    {
        _synthesizer._master_circuit_context.free_static_memory_chunk(node);
    }

    synthesizer::polyphonic_circuit_controller::polyphonic_circuit_controller(synthesizer& synth)
    :   _synthesizer{synth}
    {
    }

    void synthesizer::polyphonic_circuit_controller::compile()
    {
        LOG_INFO("[synthesizer] Compile polyphonic circuit\n");
        _synthesizer._polyphonic_circuit_context.compile({_synthesizer._midi_input}, {_synthesizer._to_master});
    }

    void synthesizer::polyphonic_circuit_controller::register_static_memory_chunk(const DSPJIT::compile_node_class &node, std::vector<uint8_t> &&data)
    {
        _synthesizer._polyphonic_circuit_context.register_static_memory_chunk(node, std::move(data));
    }

    void synthesizer::polyphonic_circuit_controller::free_static_memory_chunk(const DSPJIT::compile_node_class &node)
    {
        _synthesizer._polyphonic_circuit_context.free_static_memory_chunk(node);
    }

    /**
     *  Synthesizer implementation
     */
    synthesizer::synthesizer(
            llvm::LLVMContext &llvm_context,
            float samplerate,
            unsigned int input_count,
            unsigned int output_count,
            unsigned int voice_count,
            const opt_level level,
            const llvm::TargetOptions& options)
    :   _llvm_context{llvm_context},
        _input_count{input_count},
        _output_count{output_count},
        _master_circuit_context{_llvm_context, 1u, level, options},
        _polyphonic_circuit_context{_llvm_context, voice_count, level, options},
        _from_polyphonic{0u, voice_manager::polyphonic_to_master_channel_count},
        _output{output_count, 0u},
        _midi_input{0u, voice_manager::midi_input_count},
        _to_master{voice_manager::polyphonic_to_master_channel_count, 0u},
        _voice_manager{voice_count, _polyphonic_circuit_context},
        _master_circuit_controller{*this},
        _polyphonic_circuit_controller{*this},

        _parameter_manager{samplerate}
    {
        std::fill_n(_midi_learn_map.begin(), _midi_learn_map.size(), parameter_manager::INVALID_PARAM);
        set_sample_rate(samplerate);
    }

    void synthesizer::process_sample(const float input[], float output[]) noexcept
    {
        _process_one_sample(input, output);

        for (auto i = 0u; i < _output_count; ++i)
            output[i] *= _main_gain;
    }

    void synthesizer::process_buffer(std::size_t sample_count, const float[],float outputs[]) noexcept
    {
        for (auto i = 0u; i < sample_count; ++i)
            _process_one_sample(nullptr, outputs + i * _output_count);

        const auto buffer_size = _output_count * sample_count;
        for (auto i = 0u; i < buffer_size; ++i)
            outputs[i] *= _main_gain;
    }

    void synthesizer::midi_note_on(uint8_t note, float velocity)
    {
        if (!_voice_manager.note_on(note, velocity)) {
            LOG_ERROR("[synthesizer][not on] No anymore free voice\n");
        }
    }

    void synthesizer::midi_note_off(uint8_t note, float velocity)
    {
        _voice_manager.note_off(note);
    }

    void synthesizer::midi_control_change(uint8_t control, float value)
    {
        if (_midi_learning) {
            _midi_learn_map[control] = _learning_param;
            _midi_learning = false;
            LOG_DEBUG("[synthesizer][midi learn] Midi control %u assigned to parameter %u\n", control, _learning_param);
        }

        auto param_id = _midi_learn_map[control];
        if (param_id != parameter_manager::INVALID_PARAM)
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
                learned_param = parameter_manager::INVALID_PARAM;
        }
    }

    void synthesizer::midi_assign_control(uint8_t control, const parameter& param)
    {
        _midi_learn_map[control] = param.id();
    }

    bool synthesizer::midi_assigned_to_control(uint8_t& control, const parameter& param) const noexcept
    {
        for (auto i = 0u; i < 256; ++i) {
            if (_midi_learn_map[i] == param.id()) {
                control = i;
                return true;
            }
        }
        return false;
    }

    void synthesizer::add_library_module(std::unique_ptr<llvm::Module>&& m)
    {
        _master_circuit_context.add_library_module(llvm::CloneModule(*m));
        _polyphonic_circuit_context.add_library_module(std::move(m));
    }

    void synthesizer::set_sample_rate(float samplerate)
    {
        LOG_INFO("[synthesizer : Set sample rate to %f Hz\n", samplerate);

        _master_circuit_context.set_global_constant(_samplerate_symbol, samplerate);
        _master_circuit_controller.compile();

        _polyphonic_circuit_context.set_global_constant(_samplerate_symbol, samplerate);
        _polyphonic_circuit_controller.compile();

        _parameter_manager.set_sample_rate(samplerate);
    }

    void synthesizer::set_voice_mode(const voice_mode mode) noexcept
    {
        _voice_manager.set_voice_mode(mode);
    }

    synthesizer::voice_mode synthesizer::get_voice_mode() const noexcept
    {
        return _voice_manager.get_voice_mode();
    }

    std::size_t synthesizer::get_voice_count() const noexcept
    {
        return _polyphonic_circuit_context.get_instance_count();
    }

    void synthesizer::dump_native_code(const std::string& filename_prefix)
    {
        const uint8_t *master_code_data = nullptr;
        const uint8_t *polyphonic_code_data = nullptr;
        std::size_t master_code_size = 0u;
        std::size_t polyphonic_code_size = 0u;

        if (polyphonic_code_data = _polyphonic_circuit_context.get_native_code(polyphonic_code_size))
            _dump_native_code(filename_prefix + "_polyponic.bin", polyphonic_code_data, polyphonic_code_size);
        if (master_code_data = _master_circuit_context.get_native_code(master_code_size))
            _dump_native_code(filename_prefix + "_master.bin", master_code_data, master_code_size);
    }

    void synthesizer::enable_ir_dump(bool enable)
    {
        _master_circuit_context.enable_ir_dump(enable);
        _polyphonic_circuit_context.enable_ir_dump(enable);
    }

    bool synthesizer::update_program() noexcept
    {
        const auto b1 = _master_circuit_context.update_program();
        const auto b2 = _polyphonic_circuit_context.update_program();
        return b1 || b2; // use var in order to avoid lazy evaluation side efects
    }

    void synthesizer::_process_one_sample(const float[], float output[]) noexcept
    {
        float polyphonic_output[voice_manager::polyphonic_to_master_channel_count] = {0.f};

        //  Update parameters
        _parameter_manager.process_one_sample();

        //  Sum the outputs of each active voice
        _voice_manager.process_one_sample(polyphonic_output);

        //  Apply master processing
        _master_circuit_context.process(polyphonic_output, output);
    }

    void synthesizer::_dump_native_code(const std::string& path, const uint8_t *data, std::size_t size)
    {
        if (size > 0)
        {
            std::ofstream output{path.c_str(), std::ios::binary};
            if (output.is_open()) {
                LOG_INFO("[synthesizer] Dumping native code to '%s'\n", path.c_str());
                output.write(reinterpret_cast<const char*>(data), size);
                output.close();
            }
        }
        else
        {
            LOG_WARNING("[synthesizer][_dump_native_code] Empty native code was not dumped to '%s'.",
                path.c_str());
        }
    }
}