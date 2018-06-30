

#include <iostream>

#include "test_window.h"


int main(int argc, char **argv)
{
    Gammou::test_window window{1024, 780};
    Gammou::View::application_display display(window);

    display.open("Title");

    while (display.is_open())
        std::this_thread::sleep_for(std::chrono::seconds(1));

    std::cout << "FIN\n";

    return 0;
}