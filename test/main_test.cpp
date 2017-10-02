
#include <iostream>


#include "test_link.h"

#include "test_component.h"



#define RUN_TEST(test_function) \
	std::cout << #test_function << ":"; \
	test_function(); \
	std::cout << " OK" << std::endl



int main()
{
	RUN_TEST(test_link);
	RUN_TEST(test_component_basic);
	RUN_TEST(test_component_with_circuit_frame);
	RUN_TEST(test_frame_component_basic);
	RUN_TEST(test_frame_component_with_circuit_frame);

	return 0;
}
