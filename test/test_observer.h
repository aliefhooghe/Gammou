
#include <cassert>

#include "../src/process/observer.h"


void test_observer1()
{
	int entier = 5;

	process::subject<int> * subject =
			new process::subject<int>(&entier);

	process::observer<int> ob1, ob2;
	process::observer<int> *obdy = new process::observer<int>();


	assert( ob1.get_subject_resource() == nullptr );

	subject->register_observer(&ob1);

	assert( ob1.get_subject_resource() == &entier );

	subject->register_observer(obdy);

	assert( obdy->get_subject_resource() == &entier);

	subject->register_observer(&ob2);

	assert( ob2.get_subject_resource() == &entier);

	delete obdy;

	assert( ob1.get_subject_resource() == &entier);
	assert( ob2.get_subject_resource() == &entier);

	delete subject;

	assert( ob1.get_subject_resource() == nullptr);
	assert( ob2.get_subject_resource() == nullptr);
}

void test_observer2()
{
	int entier1;
	int entier2;

	process::subject<int> *s1
		= new process::subject<int>(&entier1);
	process::subject<int> *s2
		= new process::subject<int>(&entier2);

	process::observer<int> o1, o2, o3, o4;

	s1->register_observer(&o1);
	s1->register_observer(&o2);
	s1->register_observer(&o3);
	s1->register_observer(&o4);

	assert( o1.get_subject_resource() == &entier1 );
	assert( o2.get_subject_resource() == &entier1 );
	assert( o3.get_subject_resource() == &entier1 );
	assert( o4.get_subject_resource() == &entier1 );

	s2->register_observer(&o2);

	assert( o1.get_subject_resource() == &entier1 );
	assert( o2.get_subject_resource() == &entier2 );
	assert( o3.get_subject_resource() == &entier1 );
	assert( o4.get_subject_resource() == &entier1 );

	s2->register_observer(&o1);

	assert( o1.get_subject_resource() == &entier2 );
	assert( o2.get_subject_resource() == &entier2 );
	assert( o3.get_subject_resource() == &entier1 );
	assert( o4.get_subject_resource() == &entier1 );

	delete s1;

	assert( o1.get_subject_resource() == &entier2 );
	assert( o2.get_subject_resource() == &entier2 );
	assert( o3.get_subject_resource() == nullptr );
	assert( o4.get_subject_resource() == nullptr );

	delete s2;

	assert( o1.get_subject_resource() == nullptr );
	assert( o2.get_subject_resource() == nullptr );
	assert( o3.get_subject_resource() == nullptr );
	assert( o4.get_subject_resource() == nullptr );
}



class test_observer_callback : public process::observer<int> {

public:
	test_observer_callback() : process::observer<int>(), m_test_value(0)
	{}


	int get_test_value()
	{
		return m_test_value;
	}
protected:
	void on_subject_destruction() override
	{
		m_test_value = 42;
	}

	void on_notify(const unsigned int notification_tag) override
	{
		m_test_value = notification_tag;
	}

private:
	int m_test_value;

};

void test_observer3()
{
	// test on_tick() and on_link_destruction();

	process::subject<int> *subject = new process::subject<int>(nullptr);

	test_observer_callback test1;
	test_observer_callback test2;

	subject->register_observer(&test1);
	subject->register_observer(&test2);

	assert( test1.get_test_value() == 0 );
	assert( test2.get_test_value() == 0 );

	subject->notify_observers(183);

	assert( test1.get_test_value() == 183 );
	assert( test2.get_test_value() == 183 );

	subject->notify_observers(75);

	assert( test1.get_test_value() == 75 );
	assert( test2.get_test_value() == 75 );

	delete subject;

	assert( test1.get_test_value() == 42 );
	assert( test2.get_test_value() == 42 );
}



void test_observer()
{
	test_observer1();
	test_observer2();
	test_observer3();
}

