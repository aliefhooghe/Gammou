#ifndef ABSTRACT_FRAME_PROCESSOR_H_
#define ABSTRACT_FRAME_PROCESSOR_H_

#include "abstract_component.h"

namespace Gammou {

    namespace Process {


        template<class T>
        class abstract_frame_processor {

            public:
                abstract_frame_processor();
                virtual ~abstract_frame_processor() {}

                virtual void execute_process_program() =0;
                virtual void execute_initialize_program() =0;

                virtual void compile_component(abstract_component<T>* component) =0;
                
                void next_process_cycle();
                unsigned int get_process_cycle() const;

            protected:
                virtual void clear_program() =0;
                
                unsigned int m_process_cycle;
        };


        template<class T>
        abstract_frame_processor<T>::abstract_frame_processor()
            : m_process_cycle(42)
        {}

        template<class T>
        void abstract_frame_processor<T>::next_process_cycle()
        {
            clear_program();
            m_process_cycle++;
        }

        template<class T>
        unsigned int abstract_frame_processor<T>::get_process_cycle() const
        {
            return m_process_cycle;
        }


    } /* Process */

} /* Gammou */



#endif