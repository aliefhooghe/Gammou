#ifndef CIRCUIT_FRAME_H_
#define CIRCUIT_FRAME_H_

#include "abstract_process_frame.h"
#include "helper_components/buffer_fetcher.h"
#include "helper_components/buffer_filler.h"



namespace Gammou {

	namespace Process{


		template<class T>
		class circuit_frame : private abstract_process_frame<T> {


		public:
			circuit_frame(
				const unsigned int input_count, 
				const unsigned int output_count,
				abstract_frame_processor<T>& processor
			);
			virtual ~circuit_frame();

			using abstract_process_frame<T>::add_component;

			abstract_component<T>& get_input();
			abstract_component<T>& get_output();

			void process(const T input[], T output[]);
			using abstract_process_frame<T>::initialize_components;

			void set_input_name(const std::string& name, const unsigned int input_id);
			void set_output_name(const std::string& name, const unsigned int output_id);

		private:
			vector_fetcher_component<T> m_input;
			vector_filler_component<T> m_output;

		};


		template<class T>
		circuit_frame<T>::circuit_frame(
			const unsigned int input_count, 
			const unsigned int output_count,
			abstract_frame_processor<T>& processor)
			: abstract_process_frame<T>(processor),
				m_input(input_count), 
				m_output(output_count)
		{
			abstract_process_frame<T>::add_component(&m_input);
			abstract_process_frame<T>::add_component(&m_output);
			abstract_process_frame<T>::add_component_to_output_list(&m_output);
		}

		template<class T>
		circuit_frame<T>::~circuit_frame()
		{

		}


		template<class T>
		abstract_component<T>& circuit_frame<T>::get_input()
		{
			return m_input;
		}

		template<class T>
		abstract_component<T>& circuit_frame<T>::get_output()
		{
			return m_output;
		}

		template<class T>
		void circuit_frame<T>::process(const T *input, T *output)
		{
			m_input.set_input_buffer_ptr(input);
			m_output.set_output_pointer(output);
			abstract_process_frame<T>::process();
		}

		template<class T>
		void circuit_frame<T>::set_input_name(const std::string& name, const unsigned int input_id)
		{
			m_input.set_output_name(name, input_id);
		}

		template<class T>
		void circuit_frame<T>::set_output_name(const std::string& name, const unsigned int output_id)
		{
			m_output.set_input_name(name, output_id);
		}

	} /* Process */

} /* Gammou */


#endif /* CIRCUIT_FRAME_H_ */
