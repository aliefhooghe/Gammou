
#include <chrono>
#include <cxxopts.hpp>
#include <filesystem>
#include <optional>
#include <thread>

#include "desktop_application.h"


struct application_options
{
    bool no_gui{false};
    std::optional<std::filesystem::path> initial_patch_path{};
};

static bool parse_options(int argc, char **argv, application_options& options)
{
    // Parse command line options
    cxxopts::Options parser("Gammou", "Gammou command line interface:");

    try {
        parser.add_options()
            ("n,no-gui", "Run without gui")
            ("p,patch", "Load a patch", cxxopts::value<std::string>())
        ;

        const auto parsed_arguments = parser.parse(argc, argv);

        if (parsed_arguments.count("patch") > 0)
            options.initial_patch_path = parsed_arguments["patch"].as<std::string>();
        if (parsed_arguments.count("no-gui") > 0)
            options.no_gui = true;

        return true;
    }
    catch (std::exception& error)
    {
        LOG_INFO("%s\n", parser.help().c_str());
        LOG_ERROR("[desktop application] A fatal error occurred: %s\n", error.what());
        return false;
    }
}

static int run_desktop_application(const application_options& options)
{
    Gammou::desktop_application app{
        0u, // input count (unused)
        2u, // output count
        options.initial_patch_path};

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
    application_options options;
    if (parse_options(argc, argv, options))
        return run_desktop_application(options);
    else
        return 1;
}