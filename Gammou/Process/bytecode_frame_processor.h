#ifndef BYTECODE_FRAME_PROCESSOR_H_
#define BYTECODE_FRAME_PROCESSOR_H_

#include <vector>

#include "abstract_frame_processor.h"

namespace Gammou {

    namespace Process {

        /*
            Bytecode Instructions Definition
        */

        template<class T>
        class bytecode_instruction{

			enum class type {FETCH_OUTPUT, FETCH_DEFAULT, PROCESS};

            public:

                static bytecode_instruction fetch_output(
                                abstract_component<T> *component, 
                                const unsigned int output_id, 
                                const unsigned int mem_pos);
                static bytecode_instruction fetch_default(const unsigned int mem_pos);
                static bytecode_instruction process(abstract_component<T> *component);

                void execute(std::vector<T>& memory) const;

                ~bytecode_instruction() {};

            private:
				bytecode_instruction(
                        abstract_component<T> *const component, 
                        const unsigned int output_id,
						const unsigned int mem_pos,
						const type type);


				abstract_component<T> *const m_component;
				const unsigned int m_output_id;
				const unsigned int m_mem_pos;
				const type m_type;
		};



        template<class T>
        class bytecode_frame_processor : public abstract_frame_processor<T> {

            public:
                bytecode_frame_processor();
                ~bytecode_frame_processor() {}

                void execute_process_program() override;
                void execute_initialize_program() override;

                void compile_component(abstract_component<T>* component) override;

            private:
                void clear_program() override;

                std::vector<bytecode_instruction<T> > m_program;
                std::vector<abstract_component<T>*> m_initialization_list;
                std::vector<T> m_memory;
        };  

        /*
            Bytecode Instruction implementation
        */

       template<class T>
       bytecode_instruction<T>::bytecode_instruction(
                abstract_component<T> *const component, 
                const unsigned int output_id,
		    	const unsigned int mem_pos,
			    const type type)
            : m_component(component), 
                m_output_id(output_id), 
                m_mem_pos(mem_pos), 
                m_type(type)
        {}
        
        template<class T>
        bytecode_instruction<T> bytecode_instruction<T>::fetch_output(
                abstract_component<T> *component, 
                const unsigned int output_id, 
                const unsigned int mem_pos)
        {
            return bytecode_instruction(component, output_id, mem_pos, type::FETCH_OUTPUT);
        }

        template<class T>
        bytecode_instruction<T> bytecode_instruction<T>::fetch_default(const unsigned int mem_pos)
        {
            return bytecode_instruction(nullptr, 0, mem_pos, type::FETCH_DEFAULT);
        }

        template<class T>
        bytecode_instruction<T> bytecode_instruction<T>::process(abstract_component<T> *component)
        {
            return bytecode_instruction(component, 0, 0, type::PROCESS);
        }

        template<class T>
        inline void bytecode_instruction<T>::execute(std::vector<T>& memory) const
        {
            if( m_type == type::PROCESS )
                m_component->process(memory.data());
            else if( m_type == type::FETCH_OUTPUT )
                memory[m_mem_pos] = m_component->fetch_output(m_output_id);
            else
                memory[m_mem_pos] = T();	
        }

        /*
            bytecode_frame_processor Instruction implementation
        */

        template<class T>
        bytecode_frame_processor<T>::bytecode_frame_processor()
            : m_program(),
                m_initialization_list(),
                m_memory(16)
        {}

        template<class T>
        void bytecode_frame_processor<T>::execute_process_program()
        {
            for (const bytecode_instruction<T>& i : m_program)
                i.execute(m_memory);
        }

        template<class T>
        void bytecode_frame_processor<T>::execute_initialize_program()
        {
			for (abstract_component<T>* component : m_initialization_list) {
				component->initialize_process();
			}
        }

        template<class T>
        void bytecode_frame_processor<T>::clear_program()
        {
            m_program.clear();
            m_initialization_list.clear();
        }
        
        template<class T>
        void bytecode_frame_processor<T>::compile_component(abstract_component<T>* component)
        {
			DEBUG_PRINT("Bytecode COmpilation\n");
            if( component->update_process_cyle(abstract_frame_processor<T>::get_process_cycle()) ){
				const unsigned int ic = component->get_input_count();

				m_initialization_list.push_back(component);

				if( ic != 0){ // if there are inputs
				// todo touver une solution !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
				// todo touver une solution !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
				// todo touver une solution !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
				// todo touver une solution !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
				// todo touver une solution !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
				// todo touver une solution !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
					abstract_component<T> *src_component[16];  	// <- !!!!!!!!!!!!!!!!!
					unsigned int src_output_id[16];				// <- !!!!!!!!!!!!!!!!!

					//	Update memory size if needed
					if( ic > m_memory.size() )
						m_memory.resize(ic);

					// Get input connections info
					//	And Make program for all input components process

					for(unsigned int i = 0; i < ic; ++i){
						src_component[i] = component->get_input_src(i, src_output_id[i]);

						if(src_component[i] != nullptr)
							compile_component(src_component[i]);
					}

					//	Make 'fetch input' program

					for(unsigned int i = 0; i < ic; ++i){
						if( src_component[i] != nullptr )
							m_program.push_back(
                                bytecode_instruction<T>::fetch_output(src_component[i], src_output_id[i], i));
						else
							m_program.push_back(
                                bytecode_instruction<T>::fetch_default(i));
					}

				}

				//	Make process program

				m_program.push_back(bytecode_instruction<T>::process(component));
			}
        }

    } /* Process */

} /* Gammou */

#endif