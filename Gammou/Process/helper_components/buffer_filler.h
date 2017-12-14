#ifndef BUFFER_FILLER_H_
#define BUFFER_FILLER_H_


#include "../abstract_component.h"

namespace Gammou {

	namespace Process{

		template<class T>
		class buffer_filler_component: public abstract_component<T> {

		public:
			buffer_filler_component(const unsigned int frame_output_count) :
					abstract_component<T>("Output", frame_output_count, 0),
					m_output_pointer(nullptr)
			{}

			buffer_filler_component(const std::string& name, const unsigned int frame_output_count) :
						abstract_component<T>(name, frame_output_count, 0),
						m_output_pointer(nullptr)
			{}

			~buffer_filler_component() {
			}

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
