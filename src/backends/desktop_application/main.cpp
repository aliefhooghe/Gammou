
#include "desktop_application.h"

int main()
{
    Gammou::desktop_application app{};
    app.run();
    std::cout << "FIN" << std::endl;

    return 0;
}