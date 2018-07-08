

#include <iostream>
#include "test_window.h"


int main(int argc, char **argv)
{
    Gammou::test_window window{1024, 780};
    Gammou::View::application_display display(window);
    
	std::cout << "Opening Display\n";
    display.open("View Test Title");
	std::cout << "Wainting Display\n";
	while (display.is_open()) {
		std::cout << "Zzzzz...\n";
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}

    std::cout << "FIN\n";

    return 0;
}