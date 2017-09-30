
#ifndef LINK_H_
#define LINK_H_



namespace Gammou{

template<class T> class link_monitor;

template<class T>
class link{
	friend class link_monitor<T>;

public:
	link();
	virtual ~link();
	T *get_link_source();
	void disconnect();

protected:
	virtual void on_link_monitor_destruction() {}

private:
	void add_to_link_list(link<T> *link);
	void notify_monitor_destruction();

	link_monitor<T> *m_monitor;
	link<T> *m_previous_link;
	link<T> *m_next_link;
};


template<class T>
class link_monitor{
	friend class link<T>;

public:
	link_monitor(T * const source);
	~link_monitor();

	void plug_link(link<T> *link);

private:
	T * const m_source;
	link<T> *m_first_link;
};



template<class T>
link<T>::link() :
	m_monitor(nullptr), m_previous_link(nullptr), m_next_link(nullptr)
{
}

template<class T>
link<T>::~link()
{
	disconnect();
}

template<class T>
T *link<T>::get_link_source()
{
	if( m_monitor == nullptr )
		return nullptr;
	else
		return m_monitor->m_source;
}

template<class T>
void link<T>::disconnect()
{
	if( m_monitor != nullptr ){

		if( m_previous_link == nullptr ){
			m_monitor->m_first_link = m_next_link;

			if( m_next_link != nullptr )
				m_next_link->m_previous_link = nullptr;
		}
		else{
			m_previous_link->m_next_link = m_next_link;

			if( m_next_link != nullptr )
				m_next_link->m_previous_link = m_previous_link;
		}

	}

	m_monitor = nullptr;
	m_previous_link = nullptr;
	m_next_link = nullptr;
}

template<class T>
void link<T>::add_to_link_list(link<T> *link)
{
	if( m_next_link == nullptr ){
		link->m_previous_link = this;
		m_next_link = link;
	}
	else{
		m_next_link->add_to_link_list(link);
	}
}


template<class T>
void link<T>::notify_monitor_destruction()
{
	if( m_next_link != nullptr )
		m_next_link->notify_monitor_destruction();
	disconnect();
	on_link_monitor_destruction();
}


template<class T>
link_monitor<T>::link_monitor(T * const source) :
	m_source(source), m_first_link(nullptr)
{
}

template<class T>
link_monitor<T>::~link_monitor()
{
	if( m_first_link != nullptr )
		m_first_link->notify_monitor_destruction();
}

template<class T>
void link_monitor<T>::plug_link(link<T> *link)
{
	link->disconnect();
	link->m_monitor = this;

	if( m_first_link == nullptr )
		m_first_link = link;
	else
		m_first_link->add_to_link_list(link);
}

} /*Gammou */


#endif /* DRAFT_H_ */
