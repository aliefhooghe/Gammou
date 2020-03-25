
#include "desktop_application.h"

int main()
{
    Gammou::desktop_application app{0u,1u};
    app.run();
    std::cout << "FIN" << std::endl;

    return 0;
}