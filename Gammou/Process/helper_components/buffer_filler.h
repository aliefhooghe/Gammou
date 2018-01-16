#ifndef BUFFER_FILLER_H_
#define BUFFER_FILLER_H_


#include "../abstract_component.h"

namespace Gammou {

	namespace Process{

		template<class T>
		class vector_filler_component: public abstract_component<T> {

		public:
			vector_filler_component(const unsigned int buffer_size, T *buffer = nullptr) :
					abstract_component<T>("Vector Output", buffer_size, 0),
					m_output_pointer(buffer)
			{}

			vector_filler_component(const std::string& name, const unsigned int buffer_size, T *buffer = nullptr) :
						abstract_component<T>(name, buffer_size, 0),
						m_output_pointer(buffer)
			{}

			vector_filler_component(std::vector<T>& buffer) :
				abstract_component<T>("Vector Output", static_cast<unsigned int>(buffer.size()), 0),
				m_output_pointer(buffer.data())
			{}

			vector_filler_component(const std::string& name, std::vector<T>& buffer) :
				abstract_component<T>(name, static_cast<unsigned int>(buffer.size()), 0),
				m_output_pointer(buffer.data())
			{}

			~vector_filler_component() {}

			void set_output_pointer(T *output_pointer) {
				m_output_pointer = output_pointer;
			}

			T fetch_output(const unsigned int output_id) {
				return T();	//	It is a stub, will never be used
			}

			void process(const T input[]) {
				const unsigned int ic = abstract_component < T > ::get_input_count();

				for (unsigned int i = 0; i < ic; ++i)
					m_output_pointer[i] = input[i];
			}

			using abstract_component<T>::set_input_name;

		private:
			T *m_output_pointer;
		};

	} /* Process */

} /* Gammou */

#endif /* BUFFER_FILLER_H_ */
