#ifndef JIT_FRAME_PROCESSOR_H_
#define JIT_FRAME_PROCESSOR_H_

#include <vector>
#include <cstdint>
#include "gammou_process.h"

namespace Gammou {

    namespace Sound {


        class jit_frame_processor : public Process::abstract_frame_processor<double> {

            public:
                jit_frame_processor();
                ~jit_frame_processor();

                void execute_process_program() override;
                void execute_initialize_program() override;

                void compile_component(Process::abstract_component<double>* component) override;
                
            private:
                void compile_component_aux(
                    Process::abstract_component<double> *component,
                    const unsigned int process_cycle);
                void clear_program() override;

                /*
                    Executable memory management
                */

                void *alloc_executable(const size_t size);
                void free_executable(void *mem, const size_t size);

                /*
                    Instructions writing 
                */

                void add_program_chunk(const void *chunk, const size_t size);

                void add_mov_ptr_rax(void *ptr);
                void add_mov_ptr_rdi(void *ptr);
                void add_mov_ptr_rsi(void *ptr);
                void add_mov_ptr_rdx(void *ptr);
				void add_mov_ptr_rcx(void *ptr);
				void add_mov_ptr_r8(void *ptr);

				void add_mov_int_esi(const uint32_t value);
				void add_mov_int_rdx(const uint32_t value);

                void add_ret();
                
                void add_fetch_output(Process::abstract_component<double>* component, const unsigned int output_id, double *mem_pos);
                void add_fetch_default(double *mem_pos);
                void add_process(Process::abstract_component<double>* component, double *input);

                uint8_t *m_program;
                size_t m_program_size;

                std::vector<double> m_memory;
        };


    } /* Sound */

} /* Gammou */ 


#endif