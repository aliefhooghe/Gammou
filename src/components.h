#ifndef COMPONENTS_H_
#define COMPONENTS_H_

#include "abstract_component.h"


namespace Gammou{


template<class T>
class input_fetcher_component : public abstract_component<T> {

public:
	input_fetcher_component(const unsigned int frame_input_count)
		: abstract_component<T>("Input", 0, frame_input_count),
		  m_input_ptr(nullptr)
	{}
	~input_fetcher_component(){}

	void set_input_pointer(const T *input_pointer)
	{
		m_input_ptr = input_pointer;
	}

	T fetch_output(const unsigned int output_id)
	{
		if (m_input_ptr == nullptr )
			return T();
		else
			return m_input_ptr[output_id];
	}

	void process(const T input[]) {}


	using abstract_component<T>::set_output_name;

private:
	const T *m_input_ptr;

};

} /* Gammou */



#endif /* COMPONENT_EXEMPLE_H_ */
