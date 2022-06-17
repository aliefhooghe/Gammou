#include <algorithm>
#include <llvm/Transforms/Utils/Cloning.h>
#include <DSPJIT/log.h>

#include "synthesizer.h"

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
            const configuration& config)
    :   _llvm_context{llvm_context},
        _input_count{config.input_count},
        _output_count{config.output_count},
        _master_circuit_context{
            DSPJIT::graph_execution_context_factory::build(
                llvm_context, config.optimization_level, config.target_options)},
        _polyphonic_circuit_context{
            DSPJIT::graph_execution_context_factory::build(
                llvm_context, config.optimization_level, config.target_options, config.voice_count)},
        _from_polyphonic{0u, voice_manager::polyphonic_to_master_channel_count},
        _output{config.output_count, 0u},
        _midi_input{0u, voice_manager::midi_input_count},
        _to_master{voice_manager::polyphonic_to_master_channel_count, 0u},
        _voice_manager{config.voice_count, _polyphonic_circuit_context},
        _master_circuit_controller{*this},
        _polyphonic_circuit_controller{*this},
        _parameter_manager{config.sample_rate}
    {
        std::fill_n(_midi_learn_map.begin(), _midi_learn_map.size(), parameter_manager::INVALID_PARAM);
        set_sample_rate(config.sample_rate);
    }

    void synthesizer::process_sample(const float input[], float output[]) noexcept
    {
        _process_one_sample(input, output);
    }

    void synthesizer::process_buffer(std::size_t sample_count, const float[],float outputs[]) noexcept
    {
        for (auto i = 0u; i < sample_count; ++i)
            _process_one_sample(nullptr, outputs + i * _output_count);
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
        const auto sampleduration = 1.f / samplerate;
        LOG_INFO("[synthesizer : Set sample rate to %f Hz\n", samplerate);

        _master_circuit_context.set_global_constant(_samplerate_symbol, samplerate);
        _master_circuit_context.set_global_constant(_sample_duration_symbol, sampleduration);
        _master_circuit_controller.compile();

        _polyphonic_circuit_context.set_global_constant(_samplerate_symbol, samplerate);
        _polyphonic_circuit_context.set_global_constant(_sample_duration_symbol, sampleduration);
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
}