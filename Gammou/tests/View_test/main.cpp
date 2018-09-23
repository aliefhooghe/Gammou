

#include <iostream>
#include "test_window.h"


#include <type_traits>



int main(int argc, char **argv)
{
    Gammou::test_window window{1024, 780};
    Gammou::View::application_display display(window);

	std::cout << "Opening Display\n";
    display.open("View Test Title");
	std::cout << "Wainting Display\n";
	
	display.wait();

    std::cout << "FIN\n";

    return 0;
}

