#ifndef CIRCUIT_FRAME_H_
#define CIRCUIT_FRAME_H_

#include "abstract_frame.h"
#include "helper_components/buffer_fetcher.h"
#include "helper_components/buffer_filler.h"

namespace process{



template<class T>
class circuit_frame : public abstract_frame<T> {


public:
	circuit_frame(const unsigned int input_count, const unsigned int output_count);
	virtual ~circuit_frame();

	// from abstract frame

	void notify_circuit_change();

	void connect_component_to_output(abstract_component<T> *component, const unsigned int component_output_id, const unsigned int frame_output_id);
	void connect_input_to_component(const unsigned int frame_input_id, abstract_component<T> *component, const unsigned int component_input_id);


	void process(const T input[], T output[]);

	void set_input_name(const std::string& name, const unsigned int input_id);
	void set_output_name(const std::string& name, const unsigned int output_id);
private:
	buffer_fetcher_component<T> m_input;
	buffer_filler_component<T> m_output;

};


template<class T>
circuit_frame<T>::circuit_frame(const unsigned int input_count, const unsigned int output_count)
	: abstract_frame<T>(),
	  m_input(input_count), m_output(output_count)
{
	abstract_frame<T>::add_component(&m_input);
	abstract_frame<T>::add_component(&m_output);
}

template<class T>
circuit_frame<T>::~circuit_frame()
{

}


template<class T>
void circuit_frame<T>::notify_circuit_change()
{
	abstract_frame<T>::next_process_cycle();
	abstract_frame<T>::make_component_current_cycle_program(&m_output);
}

template<class T>
void circuit_frame<T>::connect_component_to_output(abstract_component<T> *component, const unsigned int component_output_id, const unsigned int frame_output_id)
{
	component->connect_to(component_output_id, &m_output, frame_output_id);
}


template<class T>
void circuit_frame<T>::connect_input_to_component(const unsigned int frame_input_id, abstract_component<T> *component, const unsigned int component_input_id)
{
	m_input.connect_to(frame_input_id, component, component_input_id);
}

template<class T>
void circuit_frame<T>::process(const T *input, T *output)
{
	m_input.set_input_buffer_ptr(input);
	m_output.set_output_pointer(output);
	abstract_frame<T>::execute_program();
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

} /* Gammou */


#endif /* CIRCUIT_FRAME_H_ */
