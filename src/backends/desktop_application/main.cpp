
#include "desktop_application.h"

int main()
{
    Gammou::desktop_application app{44100.f, 0u, 2u};
    app.run();
    std::cout << "FIN" << std::endl;

    return 0;
}