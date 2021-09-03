
#include "desktop_application.h"

int main(int argc, const char **argv)
{
    Gammou::desktop_application app{0u, 2u};
    app.run();
    std::cout << "FIN" << std::endl;

    return 0;
}