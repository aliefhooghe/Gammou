#ifndef COMPONENTS_H_
#define COMPONENTS_H_

#include "../abstract_component.h"


namespace process{


template<class T>
class buffer_fetcher_component : public abstract_component<T> {

public:
	buffer_fetcher_component(const unsigned int buffer_size)
		: abstract_component<T>("Input", 0, buffer_size),
		  m_buffer_ptr(nullptr)
	{}

	buffer_fetcher_component(const std::string& name, const unsigned int buffer_size)
		: abstract_component<T>(name, 0, buffer_size),
		  m_buffer_ptr(nullptr)
	{}

	~buffer_fetcher_component(){}

	void set_input_buffer_ptr(const T *input_pointer)
	{
		m_buffer_ptr = input_pointer;
	}

	T fetch_output(const unsigned int output_id)
	{
		if (m_buffer_ptr == nullptr )
			return T();
		else
			return m_buffer_ptr[output_id];
	}

	void process(const T input[]) {}


	using abstract_component<T>::set_output_name;

private:
	const T *m_buffer_ptr;

};

} /* Gammou */



#endif /* COMPONENT_EXEMPLE_H_ */
