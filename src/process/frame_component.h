#ifndef FRAME_COMPONENT_H_
#define FRAME_COMPONENT_H_

#include "abstract_component.h"
#include "abstract_frame.h"
#include "helper_components/buffer_fetcher.h"

namespace process{

template<class T>
class frame_component : public abstract_component<T>, public abstract_frame<T> {

	/*
	 *
	 */


	class output_component : public abstract_component<T> {

		friend class frame_component<T>;

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
	frame_component(const std::string& name, const unsigned int input_count, const unsigned int output_count);
	virtual ~frame_component();

	// From abstract component

	T fetch_output(const unsigned int output_id);
	void process(const T input[]);
	void initialize_process();

	void set_input_name(const std::string& name, const unsigned int input_id);
	void set_output_name(const std::string& name, const unsigned int output_id);

	// From abstract frame

	void notify_circuit_change();

	void connect_component_to_output(abstract_component<T> *component, const unsigned int component_output_id, const unsigned int frame_output_id);
	void connect_input_to_component(const unsigned int frame_input_id, abstract_component<T> *component, const unsigned int component_input_id);

private:
	buffer_fetcher_component<T> m_input;
	output_component m_output;

};



template<class T>
frame_component<T>::frame_component(const std::string& name, const unsigned int input_count, const unsigned int output_count)
	: abstract_component<T>(name, input_count, output_count),
	  abstract_frame<T>(),
	  m_input(input_count), m_output(output_count)
{
	abstract_frame<T>::add_component(&m_input);
	abstract_frame<T>::add_component(&m_output);
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
	abstract_frame<T>::execute_program();
}

template<class T>
void frame_component<T>::initialize_process()
{
	abstract_frame<T>::initialize_components();
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
void frame_component<T>::notify_circuit_change()
{
	const unsigned int ic = m_output.get_input_count();

	abstract_frame<T>::clear_program();
	abstract_frame<T>::next_process_cycle();

	for(unsigned int i = 0; i < ic; ++i){
		abstract_component<T> *src;
		src = m_output.get_input_src(i);

		if( src != nullptr)
			abstract_frame<T>::make_component_current_cycle_program(src);
	}
}

template<class T>
void frame_component<T>::connect_component_to_output(abstract_component<T> *component, const unsigned int component_output_id, const unsigned int frame_output_id)
{
	component->connect_to(component_output_id, &m_output, frame_output_id);
}

template<class T>
void frame_component<T>::connect_input_to_component(const unsigned int frame_input_id, abstract_component<T> *component, const unsigned int component_input_id)
{
	m_input.connect_to(frame_input_id, component, component_input_id);
}


} /* Gammou */



#endif /* FRAME_COMPONENT_H_ */
