#ifndef ABSTRACT_FRAME_H_
#define ABSTRACT_FRAME_H_

#include "abstract_component.h"
#include "abstract_frame_processor.h"

#define FRAME_INITIAL_MEM 16

namespace Gammou {

	namespace Process{

		template<class T>
		class abstract_frame{

		public:
			abstract_frame(abstract_frame_processor<T>& processor);
			virtual ~abstract_frame() {}

			void add_component(abstract_component<T> *component);
			virtual void notify_circuit_change() =0;

			void process();
			void initialize_components();	

		protected:
			abstract_frame_processor<T>& m_processor;
		
		private:
			subject<abstract_frame<T> > m_self_subject;
		};

		template<class T>
		abstract_frame<T>::abstract_frame(abstract_frame_processor<T>& processor)
			: m_self_subject(this),
				m_processor(processor)
		{
		}

		template<class T>
		void abstract_frame<T>::add_component(abstract_component<T> *component)
		{
			if( component->get_frame() != nullptr)
				throw std::domain_error("Can't add component that is already on a frame");

			m_self_subject.register_observer(&(component->m_frame));
			// No circtui change notification because the component is not already conected
		}

		template<class T>
		void abstract_frame<T>::initialize_components()
		{
			m_processor.execute_initialize_program();
		}


		template<class T>
		void abstract_frame<T>::process()
		{
			m_processor->execute_process_program();
		}

	} /* Process */

} /* Gammou */

#endif
