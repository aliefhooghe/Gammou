
#include <cassert>

#include "../src/process/link.h"


void test_link1()
{
	int entier = 5;

	process::link_monitor<int> * monitor =
			new process::link_monitor<int>(&entier);

	process::link<int> l1, l2;
	process::link<int> *ldy = new process::link<int>();


	assert( l1.get_link_source() == nullptr );

	monitor->plug_link(&l1);

	assert( l1.get_link_source() == &entier );

	monitor->plug_link(ldy);

	assert( ldy->get_link_source() == &entier);

	monitor->plug_link(&l2);

	assert( l2.get_link_source() == &entier);

	delete ldy;

	assert( l1.get_link_source() == &entier);
	assert( l2.get_link_source() == &entier);

	delete monitor;

	assert( l1.get_link_source() == nullptr);
	assert( l2.get_link_source() == nullptr);
}

void test_link2()
{
	int entier1;
	int entier2;

	process::link_monitor<int> *m1
		= new process::link_monitor<int>(&entier1);
	process::link_monitor<int> *m2
		= new process::link_monitor<int>(&entier2);

	process::link<int> l1, l2, l3, l4;

	m1->plug_link(&l1);
	m1->plug_link(&l2);
	m1->plug_link(&l3);
	m1->plug_link(&l4);

	assert( l1.get_link_source() == &entier1 );
	assert( l2.get_link_source() == &entier1 );
	assert( l3.get_link_source() == &entier1 );
	assert( l4.get_link_source() == &entier1 );

	m2->plug_link(&l2);

	assert( l1.get_link_source() == &entier1 );
	assert( l2.get_link_source() == &entier2 );
	assert( l3.get_link_source() == &entier1 );
	assert( l4.get_link_source() == &entier1 );

	m2->plug_link(&l1);

	assert( l1.get_link_source() == &entier2 );
	assert( l2.get_link_source() == &entier2 );
	assert( l3.get_link_source() == &entier1 );
	assert( l4.get_link_source() == &entier1 );

	delete m1;

	assert( l1.get_link_source() == &entier2 );
	assert( l2.get_link_source() == &entier2 );
	assert( l3.get_link_source() == nullptr );
	assert( l4.get_link_source() == nullptr );

	delete m2;

	assert( l1.get_link_source() == nullptr );
	assert( l2.get_link_source() == nullptr );
	assert( l3.get_link_source() == nullptr );
	assert( l4.get_link_source() == nullptr );
}



class test_link_callback : public process::link<int> {

public:
	test_link_callback() : process::link<int>(), m_test_value(0)
	{}


	int get_test_value()
	{
		return m_test_value;
	}
protected:
	void on_link_monitor_destruction()
	{
		m_test_value = 42;
	}

	void on_tick()
	{
		m_test_value++;
	}

private:
	int m_test_value;

};

void test_link3()
{
	// test on_tick() and on_link_destruction();

	process::link_monitor<int> *monitor = new process::link_monitor<int>(nullptr);

	test_link_callback test1;
	test_link_callback test2;

	monitor->plug_link(&test1);
	monitor->plug_link(&test2);

	assert( test1.get_test_value() == 0 );
	assert( test2.get_test_value() == 0 );

	monitor->send_tick();

	assert( test1.get_test_value() == 1 );
	assert( test2.get_test_value() == 1 );

	monitor->send_tick();

	assert( test1.get_test_value() == 2 );
	assert( test2.get_test_value() == 2 );

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

