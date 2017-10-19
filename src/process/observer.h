
#ifndef OBSERVER_H_
#define OBSERVER_H_


namespace process{

template<class T> class subject;

template<class T>
class observer{
	friend class subject<T>;

public:
	observer();
	virtual ~observer();
	T *get_subject_resource() const;
	void disconnect();

protected:
	virtual void on_subject_destruction() {}
	virtual void on_notify(const unsigned int notification_tag) {};

private:
	void notify_subject_destruction();
	void notify(const unsigned int notification_tag);

	subject<T> *m_subject;
	observer<T> *m_previous_observer;
	observer<T> *m_next_observer;
};


template<class T>
class subject{
	friend class observer<T>;

public:
	subject(T * const source);
	~subject();

	void register_observer(observer<T> *link);
	void notify_observers(const unsigned int notification_tag);

private:
	T * const m_resource;
	observer<T> *m_first_observer;
};



template<class T>
observer<T>::observer() :
	m_subject(nullptr), m_previous_observer(nullptr), m_next_observer(nullptr)
{
}

template<class T>
observer<T>::~observer()
{
	disconnect();
}

template<class T>
T *observer<T>::get_subject_resource() const
{
	if( m_subject == nullptr )
		return nullptr;
	else
		return m_subject->m_resource;
}

template<class T>
void observer<T>::disconnect()
{
	if( m_subject != nullptr ){

		if( m_previous_observer == nullptr ){
			m_subject->m_first_observer = m_next_observer;

			if( m_next_observer != nullptr )
				m_next_observer->m_previous_observer = nullptr;
		}
		else{
			m_previous_observer->m_next_observer = m_next_observer;

			if( m_next_observer != nullptr )
				m_next_observer->m_previous_observer = m_previous_observer;
		}

	}

	m_subject = nullptr;
	m_previous_observer = nullptr;
	m_next_observer = nullptr;
}


template<class T>
void observer<T>::notify_subject_destruction()
{
	if( m_next_observer != nullptr )
		m_next_observer->notify_subject_destruction();
	disconnect();
	on_subject_destruction();
}

template<class T>
void observer<T>::notify(const unsigned int notification_tag)
{
	if( m_next_observer != nullptr )
		m_next_observer->notify(notification_tag);
	on_notify(notification_tag);
}

/*
 *
 */

template<class T>
subject<T>::subject(T * const source) :
	m_resource(source), m_first_observer(nullptr)
{
}

template<class T>
subject<T>::~subject()
{
	if( m_first_observer != nullptr )
		m_first_observer->notify_subject_destruction();
}

template<class T>
void subject<T>::register_observer(observer<T> *link)
{
	link->disconnect();
	link->m_subject = this;

	if( m_first_observer != nullptr ){
		link->m_next_observer = m_first_observer;
		m_first_observer->m_previous_observer = link;
	}

	m_first_observer = link;
}

template<class T>
void subject<T>::notify_observers(const unsigned int notification_tag)
{
	if( m_first_observer != nullptr )
		m_first_observer->notify(notification_tag);
}

} /*Gammou */


#endif /* OBSERVER */
