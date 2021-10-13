
#include <cxxopts.hpp>
#include <optional>
#include <filesystem>

#include "desktop_application.h"


struct application_options
{
    std::optional<std::filesystem::path> initial_patch_path{};
};

static bool parse_options(int argc, char **argv, application_options& options)
{
    // Parse command line options
    cxxopts::Options parser("Gammou", "Gammou command line interface:");

    try {
        parser.add_options()
            ("p,patch", "Load a patch", cxxopts::value<std::string>())
        ;

        const auto parsed_arguments = parser.parse(argc, argv);

        if (parsed_arguments.count("patch") > 0)
        {
            options.initial_patch_path = parsed_arguments["patch"].as<std::string>();
        }

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

    app.open_display();
    app.wait_display();

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