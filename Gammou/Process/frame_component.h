#ifndef FRAME_COMPONENT_H_
#define FRAME_COMPONENT_H_

#include "abstract_component.h"
#include "abstract_process_frame.h"
#include "helper_components/buffer_fetcher.h"



namespace Gammou {

	namespace Process{

		template<class T>
		class frame_component : 
			public abstract_component<T>, 
			private abstract_process_frame<T> {

			/*
			*
			*/


			class output_component : public abstract_component<T> {

			public:
				output_component(const unsigned int frame_output_count)
					: abstract_component<T>("Output", frame_output_count, 0)
				{}
				~output_component() {}

				T fetch_output(const unsigned int output_id)
				{
					return T();	//	It is a stub, will never be used
				}

				void process(const T input[])	// A stub too
				{}

				using abstract_component<T>::set_input_name;
			};


		public:
			frame_component(
				const std::string& name, 
				const unsigned int input_count, 
				const unsigned int output_count,
				abstract_frame_processor<T>& processor);
			virtual ~frame_component();

			// From abstract component

			T fetch_output(const unsigned int output_id);
			void process(const T input[]);

			void set_input_name(const std::string& name, const unsigned int input_id);
			void set_output_name(const std::string& name, const unsigned int output_id);

			abstract_component<T>& get_input();
			abstract_component<T>& get_output();

			// From abstract process frame
			using abstract_process_frame<T>::initialize_components;
			using abstract_process_frame<T>::add_component;

		private:
			vector_fetcher_component<T> m_input;
			output_component m_output;

		};

		template<class T>
		frame_component<T>::frame_component(
			const std::string& name, 
			const unsigned int input_count, 
			const unsigned int output_count,
			abstract_frame_processor<T>& processor)
			: abstract_component<T>(name, input_count, output_count),
			abstract_process_frame<T>(processor),
			m_input(input_count), m_output(output_count)
		{
			abstract_process_frame<T>::add_component(&m_input);
			abstract_process_frame<T>::add_component(&m_output);
			abstract_process_frame<T>::add_component_to_output_list(&m_output);
		}

		template<class T>
		frame_component<T>::~frame_component()
		{
		}

		template<class T>
		T frame_component<T>::fetch_output(const unsigned int output_id)
		{
			abstract_component<T> *src_component;
			unsigned int src_output_id;

			src_component = m_output.get_input_src(output_id, src_output_id);

			if( src_component == nullptr )
				return T();
			else
				return src_component->fetch_output(src_output_id);
		}

		template<class T>
		void frame_component<T>::process(const T input[])
		{
			m_input.set_input_buffer_ptr(input);
			abstract_process_frame<T>::process();
		}


		template<class T>
		void frame_component<T>::set_input_name(const std::string& name, const unsigned int input_id)
		{
			abstract_component<T>::set_input_name(name, input_id);
			m_input.set_output_name(name, input_id);
		}

		template<class T>
		void frame_component<T>::set_output_name(const std::string& name, const unsigned int output_id)
		{
			abstract_component<T>::set_output_name(name, output_id);
			m_output.set_input_name(name, output_id);
		}

		template<class T>
		abstract_component<T>& frame_component<T>::get_input()
		{
			return m_input;
		}

		template<class T>
		abstract_component<T>& frame_component<T>::get_output()
		{
			return m_output;
		}

	} /* Process */


} /* Gammou */



#endif /* FRAME_COMPONENT_H_ */
