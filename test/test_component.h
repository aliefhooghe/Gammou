#ifndef TEST_COMPONENT_H_
#define TEST_COMPONENT_H_

#include "../src/process/abstract_component.h"
#include "../src/process/abstract_frame.h"
#include "../src/process/circuit_frame.h"
#include "../src/process/frame_component.h"


/*
template<class T>
void print_component(const abstract_component<T>& c)
{
	const unsigned int ic = c.get_input_count();
	const unsigned int oc = c.get_output_count();

	std::cout << "### Component name : " << c.get_name() << std::endl;
	std::cout << ic << " input(s)" << std::endl;

	for(unsigned int i = 0; i < ic; ++i){
		unsigned int output_id;
		const abstract_component<T> * src;
		src = c.get_input_src(i, output_id);

		std::cout << c.get_input_name(i) << " : ";

		if(src == NULL)
			std::cout << "Not connected" << std::endl;
		else
			std::cout << "Connected to '" << src->get_name()
			<< "', output N° " << output_id << " : '"
			<< src->get_output_name(output_id) << "'" << std::endl;
	}

	std::cout << oc << " output(s)" << std::endl;

	for(unsigned int i = 0; i < oc; ++i)
		std::cout << c.get_output_name(i) << std::endl;

}
*/

/*
 *
 */

template<class T>
class simple_component : public process::abstract_component<T> {

public:
	simple_component(const std::string& name, const unsigned int input_count, const unsigned int output_count)
		: process::abstract_component<T>(name, input_count, output_count)
		  , m_out(output_count)
	{}

	virtual ~simple_component() {};

	T fetch_output(const unsigned int output_id)
	{
		return m_out[output_id];
	}

protected:
	std::vector<T> m_out;
};


template<class T>
class adder_component : public simple_component<T> {

public:
	adder_component(const unsigned int input_count)
	: simple_component<T>("Add", input_count, 1)
	{}

	~adder_component() {}

	void process(const T input[])
	{
		unsigned int ic = simple_component<T>::get_input_count();

		simple_component<T>::m_out[0] = T();

		for(unsigned int i = 0; i < ic; ++i)
			simple_component<T>::m_out[0] += input[i];
	}

};

template<class T>
class mul2 : public simple_component<T> {

public:
	mul2()
	: simple_component<T>("Mul2", 1, 1)
	{}

	~mul2() {}

	void process(const T input[])
	{
		simple_component<T>::m_out[0] = 2 * input[0];
	}
};

/*
 *
 */


void test_component_basic()
{
	adder_component<int> a1(3);

	assert( a1.get_name() == "Add" );
	assert( a1.get_input_count() == 3);
	assert( a1.get_output_count() == 1);

	assert("Out-0" == a1.get_output_name(0));
	assert("In-0" == a1.get_input_name(0));
	assert("In-1" == a1.get_input_name(1));
	assert("In-2" == a1.get_input_name(2));

	assert( a1.get_input_src(0) == nullptr);
	assert( a1.get_input_src(1) == nullptr);
	assert( a1.get_input_src(2) == nullptr);

	assert( a1.get_frame() == nullptr);
}

void test_component_with_circuit_frame()
{
	process::circuit_frame<int> circuit(3, 1);

	adder_component<int> a1(4);
	mul2<int> m1;

	// add to the circuit

	circuit.add_component(&a1);
	assert( a1.get_frame() == &circuit);

	circuit.add_component(&m1);
	assert( m1.get_frame() == &circuit);

	// connection

	circuit.connect_input_to_component(0, &a1, 0);
	circuit.connect_input_to_component(1, &a1, 1);
	circuit.connect_input_to_component(2, &a1, 2);

	assert( a1.get_input_src(0) != nullptr);
	assert( a1.get_input_src(1) != nullptr);
	assert( a1.get_input_src(2) != nullptr);

	circuit.connect_component_to_output(&a1, 0, 0);

	// Try some calculation

	int input[3] = {1, 42, -10};
	int output = 44;

	circuit.process(input, &output);
	assert( output == (1 + 42 - 10));

	// New connection

	a1.connect_to(0, &m1, 0);
	circuit.connect_component_to_output(&m1, 0, 0);

	circuit.process(input, &output);
	assert( output == (1 + 42 - 10) * 2);
}

void test_frame_component_basic()
{
	process::frame_component<int> fc("Boris", 3, 2);

	adder_component<int> a1(4);
	mul2<int> m1;

	//abstract_component<int> *inside_component = &fc;

	assert( fc.get_name() == "Boris" );
	assert( fc.get_input_count() == 3 );
	assert( fc.get_output_count() == 2);

	// Fetching

	assert( fc.fetch_output(0) == int());
	assert( fc.fetch_output(1) == int());

	// Name seting

	fc.set_input_name("in0", 0);
	fc.set_output_name("ou1", 1);

	assert( fc.get_input_name(0) == "in0");
	assert( fc.get_output_name(1) == "ou1");

	//	adding component

	fc.add_component(&a1);
	fc.add_component(&m1);

	assert( a1.get_frame() == &fc);
	assert( m1.get_frame() == &fc);
}

void test_frame_component_with_circuit_frame()
{

	process::circuit_frame<int> circuit(5, 1);

	process::frame_component<int> addmul2("FC", 3, 1);

	adder_component<int> a1(2);
	mul2<int> m1;


	// Creating addmul2 component
	addmul2.add_component(&a1);
	addmul2.add_component(&m1);

	addmul2.connect_input_to_component(0, &a1, 0);
	addmul2.connect_input_to_component(1, &a1, 1);

	a1.connect_to(0, &m1, 0);

	addmul2.connect_component_to_output(&m1, 0, 0);

	// Putting addmul2 on circuit
	circuit.add_component(&addmul2);
	circuit.connect_input_to_component(0, &addmul2, 0);
	circuit.connect_input_to_component(1, &addmul2, 1);
	circuit.connect_input_to_component(2, &addmul2, 2);

	circuit.connect_component_to_output(&addmul2, 0, 0);

	// Try calculation
	int input[5] = {11, 42, 8, 2, 5};
	int output = 44;

	circuit.process(input, &output);


	assert( output == 2 * (11 + 42));

}


#endif
