#ifndef GAMMOU_SYNTHESIZER_H_
#define GAMMOU_SYNTHESIZER_H_

#include <memory>

#include <DSPJIT/compile_node_class.h>
#include <DSPJIT/graph_execution_context_factory.h>

#include "voice_manager.h"
#include "parameter_manager.h"

namespace Gammou
{
    class synthesizer {
        static constexpr auto _samplerate_symbol = "_sample_rate";
        static constexpr auto _sample_duration_symbol = "_sample_duration";

    public:

        class circuit_controller
        {
        public:
            virtual void compile() =0;
            virtual void register_static_memory_chunk(const DSPJIT::compile_node_class& node, std::vector<uint8_t>&& data) =0;
            virtual void free_static_memory_chunk(const DSPJIT::compile_node_class& node) =0;
        };

        using opt_level = DSPJIT::graph_execution_context::opt_level;
        using parameter = parameter_manager::parameter;
        using voice_mode = voice_manager::mode;

        struct configuration
        {
            float sample_rate{44100.f};
            unsigned int input_count{0u};
            unsigned int output_count{2u};
            unsigned int voice_count{256u};
            opt_level optimization_level{opt_level::Aggressive};
            llvm::TargetOptions target_options{};
        };

        /**
         *  \brief Initialize a synthesizer instance
         *  \param llvm_context llvm context instance used to perform just in time compilation
         *  \param input_count \todo (unused)
         *  \param output_count synthesizer output_count \see output_node()
         *  \param voice_count the maximum number of voice that can be played at the same time
         *  \param level Optimization level in {None, Less, Default, Aggressive}
         *  \param options native target code generation advanced options
         */
        synthesizer(
            llvm::LLVMContext& llvm_context,
            const configuration& config);

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
        void add_library_module(std::unique_ptr<llvm::Module>&& m);

        /**
         *  \brief Compile the master circuit and update the processing code
         */
        circuit_controller& get_master_circuit_controller() noexcept { return _master_circuit_controller; }

        /**
         *  \brief Compile the polyphonic circuit and update the processing code
         */
        circuit_controller& get_polyphonic_circuit_controller() noexcept { return _polyphonic_circuit_controller; }

        /**
         *  \brief Set the samplerate and recompile circuits
         */
        void set_sample_rate(float samplerate);

        /**
         * \brief Return the llvm context used for JIT compilation
         */
        auto& get_llvm_context() noexcept { return _llvm_context; }

        /**
         *  \brief Set voicing mode
         *  \param mode Voicing mode : POLYPHONIC or LEGATO
         */
        void set_voice_mode(const voice_mode mode) noexcept;

        /**
         *  \brief Return the current synthesizer voicing mode
         */
        voice_mode get_voice_mode() const noexcept;


        std::size_t get_voice_count() const noexcept;

        /**
         * \brief Enable/disable the IR code dump to logs
         */
        void enable_ir_dump(bool enable = true);

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

        void _process_one_sample(const float[], float output[]) noexcept;

        class master_circuit_controller : public circuit_controller
        {
        public:
            master_circuit_controller(synthesizer&);

            void compile() override;
            void register_static_memory_chunk(const DSPJIT::compile_node_class& node, std::vector<uint8_t>&& data) override;
            void free_static_memory_chunk(const DSPJIT::compile_node_class& node) override;

        private:
            synthesizer& _synthesizer;
        };

        class polyphonic_circuit_controller : public circuit_controller
        {
        public:
            polyphonic_circuit_controller(synthesizer&);

            void compile() override;
            void register_static_memory_chunk(const DSPJIT::compile_node_class& node, std::vector<uint8_t>&& data) override;
            void free_static_memory_chunk(const DSPJIT::compile_node_class& node) override;

        private:
            synthesizer& _synthesizer;
        };

        friend class master_circuit_controler;
        friend class polyphonic_circuit_controler;

        llvm::LLVMContext& _llvm_context;
        const unsigned int _input_count;
        const unsigned int _output_count;

        //  Circuit execution context
        DSPJIT::graph_execution_context _master_circuit_context;
        DSPJIT::graph_execution_context _polyphonic_circuit_context;

        //  Master circuit internal nodes
        DSPJIT::compile_node_class _from_polyphonic;
        DSPJIT::compile_node_class _output;

        //  Polyphonic circuit internal nodes
        DSPJIT::compile_node_class _midi_input;
        DSPJIT::compile_node_class _to_master;

        // Circuit controlers
        master_circuit_controller _master_circuit_controller;
        polyphonic_circuit_controller _polyphonic_circuit_controller;

        //  Voice management
        voice_manager _voice_manager;

        //  Parameter management
        parameter_manager _parameter_manager;
        std::array<param_id, 256u> _midi_learn_map;
        bool _midi_learning{false};
        param_id _learning_param;
    };

} // namespace Gammou


#endif