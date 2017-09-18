

#include "component.h"

namespace Gammou {

template<class T>
component_link<T>::component_link() throw ():
		m_src(NULL), m_out_id(0), m_next_link(NULL), m_previous_link(NULL)
{

}

template<class T>
void component_link<T>::add_to_link_list(component_link *link) throw()
{
	if(m_next_link == NULL){
		m_next_link = link;
		link->m_previous_link = this;
	}
	else{
		m_next_link->add_to_link_list(link);
	}
}

template<class T>
void component_link<T>::notify_src_destruction() throw ()
{
	m_src = NULL;
	m_out_id = 0;
	m_previous_link = NULL;

	if(m_next_link != NULL){
		m_next_link->notify_src_destruction();
		m_next_link = NULL;
	}
}

///

template<class T>
component<T>::component(const std::string& name, const unsigned int input_count,
		const unsigned int output_count)
	: m_input(input_count), m_first_link(NULL), m_input_name(input_count),
		  m_output_name(output_count), m_name(name)
{
	for (unsigned int i = 0; i < input_count; ++i)
		m_input_name[i] = input_default_name(i);

	for (unsigned int i = 0; i < output_count; ++i)
		m_output_name[i] = output_default_name(i);
}

template<class T>
component<T>::~component()
{
	const unsigned int ic = get_input_count();

	for(unsigned i = 0; i < ic; ++i)
		disconnect_link(&(m_input[i]));

	if(m_first_link != NULL)
		m_first_link->notify_src_destruction();
}

template<class T>
const unsigned int component<T>::get_input_count() const throw ()
{
	return m_input_name.size();
}

template<class T>
const unsigned int component<T>::get_output_count() const throw ()
{
	return m_output_name.size();
}

template<class T>
const std::string component<T>::get_name() const
{
	return m_name;
}

template<class T>
const std::string component<T>::get_input_name(const unsigned int input_id) const throw (invalid_id)
{
	if(input_id >= get_input_count())
		throw invalid_id(input_id);

	return m_input_name[input_id];
}

template<class T>
const std::string component<T>::get_output_name(const unsigned int output_id) const throw (invalid_id)
{
	if(output_id >= get_output_count())
		throw invalid_id(output_id);

	return m_output_name[output_id];
}

template<class T>
void component<T>::connect_to(const unsigned int output_id, component *dst,
		const unsigned int dst_input_id) throw(invalid_id)
{
	if(output_id >= get_output_count()
			|| dst_input_id >= dst->get_input_count())
		throw invalid_id();


	connect_to_link(&(dst->m_input[dst_input_id]), output_id);
}

template<class T>
component<T> *component<T>::get_input_src(const unsigned int input_id,  unsigned int& output_id) const
		throw(invalid_id)
{
	if(input_id >= get_input_count())
		throw invalid_id(input_id);

	output_id = m_input[input_id].m_out_id;
	return m_input[input_id].m_src;
}

// protected

template<class T>
void component<T>::pop_input() throw (impossible_plug_pop)
{
	const unsigned int ic = get_input_count();

	if(ic == 0)
		throw impossible_plug_pop();

	disconnect_link(&(m_input[ic - 1]));
	m_input.pop_back();
	m_input_name.pop_back();
}

template<class T>
void component<T>::push_input(const std::string& name)
{
	m_input.push_back(component_link<T>());
	m_input_name.push_back(name);
}

template<class T>
void component<T>::push_input()
{
	const unsigned int n = get_input_count();
	m_input.push_back(component_link<T>());
	m_input_name.push_back(input_default_name(n));
}

template<class T>
void component<T>::set_input_name(const std::string& name, const unsigned int input_id) throw (invalid_id)
{
	if(input_id >= get_input_count())
		throw invalid_id(input_id);

	m_input_name[input_id] = name;
}

template<class T>
void component<T>::set_output_name(const std::string& name, const unsigned int output_id) throw (invalid_id)
{
	if(output_id >= get_output_count())
		throw invalid_id(output_id);

	m_output_name[output_id] = name;
}

// private

template<class T>
void component<T>::connect_to_link(component_link<T> *link, const unsigned int output_id)
{

	if(link->m_src != this){

		disconnect_link(link);

		link->m_src = this;
		link->m_next_link = NULL;
		link->m_out_id = output_id;

		if(m_first_link == NULL){
			link->m_previous_link = NULL;
			m_first_link = link;
		}
		else{
			m_first_link->add_to_link_list(link);
		}
	}
}

template<class T>
void component<T>::disconnect_link(component_link<T> *link)
{
	if(link->m_src != NULL){
		if(link->m_previous_link == NULL){
			link->m_src->m_first_link = link->m_next_link;
		}
		else{
			link->m_previous_link->m_next_link = link->m_next_link;
			link->m_previous_link = NULL;
		}

		link->m_next_link = NULL;
		link->m_src = NULL;
	}


}

template<class T>
const std::string component<T>::input_default_name(const unsigned int input_id)
{
	return "In-" + std::to_string(input_id);
}

template<class T>
const std::string component<T>::output_default_name(const unsigned int output_id)
{
	return "Out-" + std::to_string(output_id);
}


}
