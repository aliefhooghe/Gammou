
#include <chrono>
#include <thread>

#include <DSPJIT/log.h>
#include "application_options.h"
#include "argument_parser.h"

static int run_desktop_application(const Gammou::application_options& options)
{
    Gammou::desktop_application app{options.configuration};

    if (options.no_gui) {
        for (;;) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    }
    else {
        app.open_display();
        app.wait_display();
    }

    std::cout << "FIN" << std::endl;
    return 0;
}

int main(int argc, char **argv)
{
    Gammou::application_options options;
    if (Gammou::parse_options(argc, argv, options))
        return run_desktop_application(options);
    else
        return 1;
}
