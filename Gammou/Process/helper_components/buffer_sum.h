#ifndef BUFFER_SUM_H_
#define BUFFER_SUM_H_

#include "../abstract_component.h"

namespace Gammou {

	namespace Process{


		template<class T>
		class buffer_sum_component : public abstract_component<T> {

		public:
			buffer_sum_component(const unsigned int buffer_size)
				: abstract_component<T>("Output", buffer_size, 0),
				m_buffer_ptr(nullptr)
			{}

			buffer_sum_component(const std::string& name, const unsigned int buffer_size)
				: abstract_component<T>(name, buffer_size, 0),
				m_buffer_ptr(nullptr)
			{}


			void set_buffer_ptr(T *buffer_ptr)
			{
				m_buffer_ptr = buffer_ptr;
			}

			T fetch_output(const unsigned int output_id) { return T(); } // a stub

			void process(const T input[])
			{
				const unsigned int ic = abstract_component<T>::get_input_count();

				if( m_buffer_ptr == nullptr)
					return;

				for(unsigned int i = 0; i < ic; ++i)
					m_buffer_ptr[i] += input[i];
			}

			using abstract_component<T>::set_input_name;

		private:
			T *m_buffer_ptr;

		};

	} /* Process */

} /* Gammou */


#endif /* BUFFER_SUM_H_ */
