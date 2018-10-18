#ifndef ABSTRACT_PROCESS_FRAME_H
#define ABSTRACT_PROCESS_FRAME_H

#include <vector>

#include "abstract_frame.h"
#include "abstract_frame_processor.h"


namespace Gammou {

    namespace Process {

        //  TODO : this class is not anymore abstract : rename it
        template<class T>
        class abstract_process_frame : public abstract_frame<T> {

            public:
                abstract_process_frame(abstract_frame_processor<T>& processor);

                virtual void notify_circuit_change() override;

                void clear_output_list();
                void add_component_to_output_list(abstract_component<T>* component);
            
                void process();
                void initialize_components();
            
            private:
                abstract_frame_processor<T>& m_processor;
                std::vector<abstract_component<T>*> m_output_components;
        };

        template<class T>
        abstract_process_frame<T>::abstract_process_frame(abstract_frame_processor<T>& processor)
            : m_processor(processor),
                m_output_components()  //  avoid reallocation in most cases
        {}

        template<class T>
        void abstract_process_frame<T>::notify_circuit_change()
        {
            m_processor.next_process_cycle();
            for (abstract_component<T> *component: m_output_components)
                m_processor.compile_component(component);
        }

        template<class T>
        void abstract_process_frame<T>::clear_output_list()
        {
            m_output_components.clear();
        }

        template<class T>
        void abstract_process_frame<T>::add_component_to_output_list(abstract_component<T>* component)
        {
            const abstract_frame<T> *frame = component->get_frame();

            if (frame != this)
                throw std::domain_error("This Component is not on this frame");

            m_output_components.push_back(component);
			notify_circuit_change();
        }

        template<class T>
        void abstract_process_frame<T>::process()
        {
            m_processor.execute_process_program();
        }

        template<class T>
        void abstract_process_frame<T>::initialize_components()
        {
            m_processor.execute_initialize_program();
        }

        

    } /* Process */

} /* Gammou */


#endif
