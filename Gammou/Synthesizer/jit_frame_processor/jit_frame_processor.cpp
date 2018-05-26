
#include "jit_frame_processor.h"

#include <cstring>
#include <cstdint>
#include <sys/mman.h>

#define PROGRAM_SIZE 8192

namespace Gammou {

    namespace Sound {

        jit_frame_processor::jit_frame_processor() 
         : m_memory(42)
        {
            m_program = (uint8_t*)alloc_executable(PROGRAM_SIZE);
            clear_program();
        }

        jit_frame_processor::~jit_frame_processor()
        {
            free_executable(m_program, PROGRAM_SIZE);
        }

        void jit_frame_processor::execute_process_program()
        {
            void (*fct)() = (void (*)())m_program;
            fct();
        }

        void jit_frame_processor::execute_initialize_program()
        {
            // TODO
        }

        void jit_frame_processor::compile_component(Process::abstract_component<double>* component)
        {
            const unsigned int current_process_cycle = get_process_cycle();

            // A ret Instruction is always at the end of the program, lets delete it
            m_program_size--;

            // Generate code for the component
            compile_component_aux(component, current_process_cycle);

            // Reput ret instruction at the end of the new program
            add_ret();

            DEBUG_PRINT("JIT Compililation : code size = %lu\n", m_program_size);
        }

        void jit_frame_processor::compile_component_aux(
            Process::abstract_component<double>* component,
            const unsigned int process_cycle)
        {
            if (component->update_process_cyle(process_cycle)) {
                const unsigned int ic = component->get_input_count();

                // TODO Initialization list

                if (ic != 0) {
                    Process::abstract_component<double> *src_component[16];
                    unsigned int src_output_id[16];

                    // Compile each component pluged on input

                    for (unsigned int i = 0; i < ic; ++i) {
                        src_component[i] = component->get_input_src(i, src_output_id[i]);

                        if (src_component[i] != nullptr)
                            compile_component_aux(src_component[i], process_cycle);
                    }

                    // Fetch components output values

                    for (unsigned int i = 0; i < ic; ++i) {
                        if (src_component[i] != nullptr)
                            add_fetch_output(src_component[i], src_output_id[i], &(m_memory[i]));
                        else
                            add_fetch_default(&(m_memory[i]));
                    }

                }

                // 
                add_process(component, m_memory.data());
            }
        }

        void jit_frame_processor::clear_program()
        {
            m_program_size = 0;
            // Add return Instruction
            add_ret();
        }

        /*
            Executable memory management
        */

        void *jit_frame_processor::alloc_executable(const size_t size)
        {
            void *ret = mmap(
                0, size, 
                PROT_READ | PROT_WRITE | PROT_EXEC,
                MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

            if (ret == (void*)-1)
                throw std::runtime_error("mmap failed");

            return ret;
        }

        void jit_frame_processor::free_executable(void *mem, const size_t size)
        {
            munmap(mem, size);
        }

        /*
            Instruction Writing
        */

       void jit_frame_processor::add_program_chunk(const void *chunk, const size_t size)
       {
           std::memcpy(m_program + m_program_size, chunk, size);
           m_program_size += size;
       }

        void jit_frame_processor::add_mov_ptr_rdi(void *ptr)
        {
            const uint16_t code_chunk = 0xbf48;
            add_program_chunk(&code_chunk, 2);
            add_program_chunk(&ptr, 8);
        }

        void jit_frame_processor::add_mov_ptr_rsi(void *ptr)
        {
            const uint16_t code_chunk = 0xbe48;
            add_program_chunk(&code_chunk, 2);
            add_program_chunk(&ptr, 8);
        }

        void jit_frame_processor::add_mov_ptr_rdx(void *ptr)
        {
            const uint16_t code_chunk = 0xba48;
            add_program_chunk(&code_chunk, 2);
            add_program_chunk(&ptr, 8);
        }

        void jit_frame_processor::add_mov_int_esi(const uint32_t value)
        {
            const uint8_t code_chunk = 0xbe;
            add_program_chunk(&code_chunk, 1);
            add_program_chunk(&value, 4);
        }

        void jit_frame_processor::add_ret()
        {
            const uint8_t code_chunk = 0xc3;
            add_program_chunk(&code_chunk, 1);
        }

        void jit_frame_processor::add_fetch_output(
            Process::abstract_component<double>* component, 
            const unsigned int output_id, 
            double *mem_pos)
        {
            const uint8_t code_chunk[] = 
            {
                0x48, 0x8b, 0x07, 0x53,
                0x48, 0x89, 0xd3, 0xff,
                0x50, 0x10, 0xf2, 0x0f,
                0x11, 0x03, 0x5b
            };

            add_mov_ptr_rdi(component);
            add_mov_int_esi(output_id);
            add_mov_ptr_rdx(mem_pos);
            add_program_chunk(code_chunk, sizeof(code_chunk));
        }

        void jit_frame_processor::add_fetch_default(double *mem_pos)
        {
            const uint8_t code_chunk[] = 
            {
                0x48, 0xc7, 0x07, 0x00, 
                0x00, 0x00, 0x00
            };

            add_mov_ptr_rdi(mem_pos);
            add_program_chunk(code_chunk, sizeof(code_chunk));
        }

        void jit_frame_processor::add_process(Process::abstract_component<double>* component, double *input)
        {
            const uint16_t code_chunk[] =
            {
                0x8b48, 0xff07, 0x1850
            };

            add_mov_ptr_rdi(component);
            add_mov_ptr_rsi(input);
            add_program_chunk(code_chunk, sizeof(code_chunk));
        }



    } /* Sound */

} /* Gammou */ 