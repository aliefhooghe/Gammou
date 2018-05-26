#ifndef ABSTRACT_FRAME_H_
#define ABSTRACT_FRAME_H_

#include "abstract_component.h"


namespace Gammou {

	namespace Process{

		template<class T>
		class abstract_frame{

		public:
			abstract_frame();
			virtual ~abstract_frame() {}

			void add_component(abstract_component<T> *component);
			virtual void notify_circuit_change() =0;
		
		private:
			subject<abstract_frame<T> > m_self_subject;
		};

		template<class T>
		abstract_frame<T>::abstract_frame()
			: m_self_subject(this)
		{
		}

		template<class T>
		void abstract_frame<T>::add_component(abstract_component<T> *component)
		{
			if( component->get_frame() != nullptr)
				throw std::domain_error("Can't add component that is already on a frame");

			m_self_subject.register_observer(&(component->m_frame));
			// No circuit change notification because the component is not already conected
		}

	} /* Process */

} /* Gammou */

#endif
