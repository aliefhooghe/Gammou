#ifndef MULTI_CHANNEL_DATA_CONTAINER_H_
#define MULTI_CHANNEL_DATA_CONTAINER_H_

#include <vector>


#include "channels_manager.h"


namespace gammou {


/*
 * 	Base implementation for all multi-channel container
 */

class abstract_sound_component;

class multi_channel_data {

public:
	multi_channel_data(abstract_sound_component *owner);
	virtual ~multi_channel_data() {};

protected:
	unsigned int get_channels_count();
	unsigned int get_current_working_channel();

private:
	unsigned int m_channels_count;
	abstract_sound_component *m_owner;
};




/*
 * 		multi channel variable
 */

template<class T>
class multi_channel_variable : private multi_channel_data{

public:
	multi_channel_variable(abstract_sound_component *owner);
	virtual ~multi_channel_variable() {}

	T operator =(const T& value);
	operator T();

protected:
	std::vector<T> m_data;
};

template<class T>
multi_channel_variable<T>::multi_channel_variable(abstract_sound_component *owner)
	: multi_channel_data(owner),
	  m_data(get_channels_count())
{
}

template<class T>
T multi_channel_variable<T>::operator=(const T& value)
{
	m_data[get_current_working_channel()] = value;
}

template<class T>
multi_channel_variable<T>::operator T()
{
	return m_data[get_current_working_channel()];
}

/*
 * 		multi channel array
 */

template<class T>
class multi_channel_array : private multi_channel_data{

public:
	multi_channel_array(abstract_sound_component *owner, const unsigned int size);
	virtual ~multi_channel_array() {}

	T& operator [](const unsigned int index);
	unsigned int length();
private:
	const unsigned int m_length;
	std::vector<T> m_data;
};

template<class T>
multi_channel_array<T>::multi_channel_array(abstract_sound_component *owner, const unsigned int size)
	: multi_channel_data(owner),
	  m_length(size),
	  m_data(get_channels_count() * size)
{
}

template<class T>
T& multi_channel_array<T>::operator [](const unsigned int index)
{
	// TODO Verif ou perf ???
	const unsigned int base = m_length * multi_channel_data::get_current_working_channel();
	return m_data[base + index];
}




}





#endif /* MULTI_CHANNEL_DATA_MANAGER_H_ */
