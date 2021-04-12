#ifndef GAMMOU_SYNTHESIZER_H_
#define GAMMOU_SYNTHESIZER_H_

#include <memory>

#include <compile_node_class.h>
#include <graph_execution_context.h>
#include <object_dumper.h>

#include "voice_manager.h"
#include "parameter_manager.h"

namespace Gammou
{
    class synthesizer {
        static constexpr auto samplerate_symbol = "_sample_rate";
    public:

        /**
         *  \enum midi_inputs The available channel midi inputs
         */
        enum midi_inputs {
            gate = 0u,          /** 1. when note is on, 0. otherwise **/
            pitch,              /** the pitch of the note being played (Hz) **/
            attack,             /** the attack velocity in [0., 1.] **/

            midi_input_count
        };

        static constexpr auto polyphonic_to_master_channel_count = 2u;
        static constexpr auto voice_disappearance_treshold = 0.0003f;

        using opt_level = DSPJIT::graph_execution_context::opt_level;
        using parameter = parameter_manager::parameter;

        /**
         *  \brief Initialize a synthesizer instance
         *  \param llvm_context llvm context instance used to perform just in time compilation
         *  \param input_count \todo
         *  \param output_count synthesizer output_count \see output_node()
         *  \param voice_count the maximum number of voice that can be played at the same time
         *  \param level Optimization level in {None, Less, Default, Aggressive}
         *  \param options native target code generation advanced options
         */
        synthesizer(
            llvm::LLVMContext &llvm_context,
            float samplerate = 48000.f,
            unsigned int input_count = 0u,
            unsigned int output_count = 2u,
            unsigned int voice_count = 256,
            const opt_level level = opt_level::Aggressive,
            const llvm::TargetOptions& options = llvm::TargetOptions{});

        synthesizer(const synthesizer&) = delete;
        synthesizer(synthesizer&&) = delete;
        ~synthesizer() = default;

        /**
         *  \brief Return the number of inputs
         */
        auto get_input_count() const noexcept { return _input_count; }

        /**
         *  \brief Return the number of outputs
         */
        auto get_output_count() const noexcept { return _output_count; }

        /*
         *  Master circuit internal nodes
         */
        auto& from_polyphonic_node() noexcept { return _from_polyphonic; }
        auto& output_node() noexcept { return _output; }

        /*
         *  Polyphonic circuit internal nodes
         */
        auto& midi_input_node() noexcept { return _midi_input; }
        auto& to_master_node() noexcept { return _to_master; }

        /**
         *  \brief Add module and make it available for synthesizer circuits nodes
         */
        void add_module(std::unique_ptr<llvm::Module>&& m);

        /**
         *  \brief Compile the master circuit and update the processing code
         */
        void compile_master_circuit();

        /**
         *  \brief Compile the polyphonic circuit and update the processing code
         */
        void compile_polyphonic_circuit();

        /**
         **
         **    Process thread part
         **
         **     This part can be safely used concurrently with the previous methods,
         **     typically in a sound processing thread, while gui thread access the compilation and graph editing interface.
         **/

        /**
         *  \brief Switch to the next available compiled program
         */
        bool update_program() noexcept;

        /**
         *  \brief compute one output sample using one input sample
         *  \param input input values [channel0, channel1, ..., channelN]
         *  \param output output values [channel0, channel1, ..., channelN]
         */
        void process_sample(
            const float input[],
            float output[]) noexcept;

        /**
         *  \brief compute N output samples using N input samples
         *  \param sample_count the number of sample to be computed
         *  \param inputs input buffer [ch0, ch1, ..., chN, ch0, ch1, ..., chN, ...]
         *  \param outputs output buffer [ch0, ch1, ..., chN, ch0, ch1, ..., chN, ...]
         */
        void process_buffer(
            std::size_t sample_count,
            const float inputs[],
            float outputs[]) noexcept;

        /**
         *  \brief handle a midi note on event
         *  \param note midi note played in [0, 127]
         *  \param velocity midi velocity in [0., 1.]
         */
        void midi_note_on(uint8_t node, float velocity);

        /**
         *  \brief handle a midi note off event
         *  \param note midi note played in [0, 127]
         *  \param velocity midi velocity in [0., 1.]
         */
        void midi_note_off(uint8_t node, float velocity);

        /**
         *  \brief handle a midi control change event
         *  \param control the control id in \todo
         *  \param value the new control value in [0., 1.]
         */
        void midi_control_change(uint8_t control, float value);

        /**
         *  \brief allocate a new parameter
         *  \param initial_value the value at which the new parameter is initialized
         *  \return the new parameter if allocation was succeffull, throw an exception otherwise
         */
        parameter allocate_parameter(float initial_value = 0.f);

        /**
         *  \brief Assign this parameter to the next midi control whose value change
         *  \param param the parameter to be linked to a midi control
         *  \note This will cancel any midi learning that was not terminated
         */
        void midi_learn(const parameter& param);

        /**
         *  \brief Disassign this parameter to any midi control
         *  \param param the parameter to be unlinked
         *  \note This will cancel any midi learning that was not terminated for that parameter
         */
        void midi_disassign(const parameter& param);

        /**
         *  \brief Assign a control to a parameter
         *  \param control
         *  \param parameter
         */
        void midi_assign_control(uint8_t control, const parameter& param);

        /**
         *  \param control reference used to return the control linked to the parameter if any
         *  \param param  the parameter
         *  \return true if the parameter is assigned the given midi control
         */
        bool midi_assigned_to_control(uint8_t& control, const parameter& param) const noexcept;

    private:
        using param_id = parameter_manager::param_id;

        void _initialize_samplerate_variable();

        void _process_one_sample(const float[], float output[]) noexcept;

        auto get_voice_midi_input(voice_manager::voice voice) noexcept
        {
            return _midi_input_values.data() + voice * midi_input_count;
        }

        llvm::LLVMContext& _llvm_context;
        const unsigned int _input_count;
        const unsigned int _output_count;
        float _sample_rate;

        //  Midi input values are stored here for every running voices
        std::vector<float> _midi_input_values;

        //  Circuit execution context
        DSPJIT::graph_execution_context _master_circuit_context;
        DSPJIT::graph_execution_context _polyphonic_circuit_context;

        //  Master circuit internal nodes
        DSPJIT::compile_node_class _from_polyphonic;
        DSPJIT::compile_node_class _output;

        //  Polyphonic circuit internal nodes
        DSPJIT::compile_node_class _midi_input;
        DSPJIT::compile_node_class _to_master;

        //  Voice management
        voice_manager _voice_manager;
        unsigned int _voice_disappearance_sample_count{20000u};
        std::vector<unsigned int> _voice_lifetime;

        //  Parameter management
        parameter_manager _parameter_manager{48000.f};
        std::array<param_id, 256u> _midi_learn_map;
        bool _midi_learning{false};
        param_id _learning_param;

        //  Main settings
        float _volume{0.2f};
    };

} // namespace Gammou


#endif