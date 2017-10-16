
#include <cassert>

#include "../src/process/observer.h"


void test_link1()
{
	int entier = 5;

	process::subject<int> * monitor =
			new process::subject<int>(&entier);

	process::observer<int> l1, l2;
	process::observer<int> *ldy = new process::observer<int>();


	assert( l1.get_subject_resource() == nullptr );

	monitor->register_observer(&l1);

	assert( l1.get_subject_resource() == &entier );

	monitor->register_observer(ldy);

	assert( ldy->get_subject_resource() == &entier);

	monitor->register_observer(&l2);

	assert( l2.get_subject_resource() == &entier);

	delete ldy;

	assert( l1.get_subject_resource() == &entier);
	assert( l2.get_subject_resource() == &entier);

	delete monitor;

	assert( l1.get_subject_resource() == nullptr);
	assert( l2.get_subject_resource() == nullptr);
}

void test_link2()
{
	int entier1;
	int entier2;

	process::subject<int> *m1
		= new process::subject<int>(&entier1);
	process::subject<int> *m2
		= new process::subject<int>(&entier2);

	process::observer<int> l1, l2, l3, l4;

	m1->register_observer(&l1);
	m1->register_observer(&l2);
	m1->register_observer(&l3);
	m1->register_observer(&l4);

	assert( l1.get_subject_resource() == &entier1 );
	assert( l2.get_subject_resource() == &entier1 );
	assert( l3.get_subject_resource() == &entier1 );
	assert( l4.get_subject_resource() == &entier1 );

	m2->register_observer(&l2);

	assert( l1.get_subject_resource() == &entier1 );
	assert( l2.get_subject_resource() == &entier2 );
	assert( l3.get_subject_resource() == &entier1 );
	assert( l4.get_subject_resource() == &entier1 );

	m2->register_observer(&l1);

	assert( l1.get_subject_resource() == &entier2 );
	assert( l2.get_subject_resource() == &entier2 );
	assert( l3.get_subject_resource() == &entier1 );
	assert( l4.get_subject_resource() == &entier1 );

	delete m1;

	assert( l1.get_subject_resource() == &entier2 );
	assert( l2.get_subject_resource() == &entier2 );
	assert( l3.get_subject_resource() == nullptr );
	assert( l4.get_subject_resource() == nullptr );

	delete m2;

	assert( l1.get_subject_resource() == nullptr );
	assert( l2.get_subject_resource() == nullptr );
	assert( l3.get_subject_resource() == nullptr );
	assert( l4.get_subject_resource() == nullptr );
}



class test_link_callback : public process::observer<int> {

public:
	test_link_callback() : process::observer<int>(), m_test_value(0)
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

void test_link3()
{
	// test on_tick() and on_link_destruction();

	process::subject<int> *monitor = new process::subject<int>(nullptr);

	test_link_callback test1;
	test_link_callback test2;

	monitor->register_observer(&test1);
	monitor->register_observer(&test2);

	assert( test1.get_test_value() == 0 );
	assert( test2.get_test_value() == 0 );

	monitor->notify_observers(183);

	assert( test1.get_test_value() == 183 );
	assert( test2.get_test_value() == 183 );

	monitor->notify_observers(75);

	assert( test1.get_test_value() == 75 );
	assert( test2.get_test_value() == 75 );

	delete monitor;

	assert( test1.get_test_value() == 42 );
	assert( test2.get_test_value() == 42 );
}



void test_link()
{
	test_link1();
	test_link2();
	test_link3();
}

