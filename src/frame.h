#ifndef FRAME_H_
#define FRAME_H_

#include "component.h"

#include <iostream>

namespace Gammou{

template<class T>
class frame_input : public component<T>{

public:
	frame_input(const unsigned int n_input, frame<T> *f)
		: component<T>("Input", 0, n_input, f), m_input_data(NULL)
	{}

	T fetch_output(const unsigned int output_id)
	{
		if(m_input_data == NULL)
			return T();
		else
			return m_input_data[output_id];
	}

	void process(const T input[])
	{}

	void set_input_data(const T *input_data)
	{
		m_input_data = input_data;
	}

private:
	const T *m_input_data;
};

template<class T>
class frame_output : public component<T>{

public:
	frame_output(const unsigned int n_output, frame<T> *f)
		: component<T>("Output", n_output, 0, f), m_output_data(NULL)
	{}

	T fetch_output(const unsigned int output_id)
	{
		return T();
	}

	void process(const T input[])
	{
		if(m_output_data != NULL){
			const unsigned int oc = component<T>::get_output_count();
			for(unsigned int i = 0; i <= oc; ++i){
				m_output_data[i] = input[i];
			}
		}
	}

	void set_output_data(T *output_data)
	{
		m_output_data = output_data;
	}

private:
	T *m_output_data;

};

template<class T>
class frame{

public:
	frame(const unsigned int n_input, const unsigned int n_output);

	void connect_input_to_component(const unsigned int input_id, component<T> *dst, const unsigned int dst_input_id);
	void connect_component_to_output(component<T> *src, const unsigned int src_output_id, const unsigned int output_id);


	//void notify_circuit_change();

	void process_rec(const T *inputs, T *outputs);

protected:

	//void pop_input() throw (impossible_plug_pop);
	//void push_input(const std::string& name);
	//void push_input();

	//void set_input_name(const std::string& name, const unsigned int input_id) throw (component<T>::invalid_id);
	//void set_output_name(const std::string& name, const unsigned int output_id) throw (invalid_id);


private:
	void make_component_process(component<T> *c);

	frame_input<T> m_input;
	frame_output<T> m_output;
	unsigned int m_cycle;
};




/*
 *
 *
 *
 */

template<class T>
frame<T>::frame(const unsigned int n_input, const unsigned int n_output)
	: m_input(n_input, this), m_output(n_output, this), m_cycle(0)
{

}




template<class T>
void frame<T>::connect_input_to_component(const unsigned int input_id, component<T> *dst, const unsigned int dst_input_id)
{
	m_input.connect_to(input_id, dst, dst_input_id);
}

template<class T>
void frame<T>::connect_component_to_output(component<T> *src, const unsigned int src_output_id, const unsigned int output_id)
{
	src->connect_to(src_output_id, &(m_output), output_id);
}


template<class T>
void frame<T>::process_rec(const T *inputs, T *outputs)
{
	m_cycle++;
	m_input.set_input_data(inputs);
	m_output.set_output_data(outputs);
	make_component_process(&(m_output));
}

template<class T>
void frame<T>::make_component_process(component<T> *c)
{
	const unsigned int ic = c->get_input_count();

	if( ic != 0 ){
		T inputs[ic];

		for(unsigned int i = 0; i < ic; ++i){
			unsigned int src_output_id;
			component<T> *src;

			src = c->get_input_src(i, src_output_id);

			if(src == NULL){
				inputs[i] = T();
			}
			else{
				if(src->update_cyle(m_cycle))
					make_component_process(src);

				inputs[i] = src->fetch_output(src_output_id);
			}

		}

		c->process(inputs);
	}
	else{
		c->process(NULL);
	}
}




} /* Gammou */

#endif
