
#include <chrono>
#include <cxxopts.hpp>
#include <filesystem>
#include <optional>
#include <thread>

#include "backends/common/default_configuration.h"
#include "desktop_application.h"


struct application_options
{
    bool no_gui{false};
    Gammou::application::configuration application_configuration;
    std::optional<std::filesystem::path> initial_patch_path{};
};

static bool parse_options(int argc, char **argv, application_options& options)
{
    // Parse command line options
    cxxopts::Options parser("Gammou", "Gammou command line interface:");

    constexpr auto no_gui_opt = "no-gui";
    constexpr auto patch_opt = "patch";
    constexpr auto package_path_opt = "packages-path";
    constexpr auto patch_path_opt = "patchs-path";

    try {
        parser.add_options()
            (no_gui_opt, "Run without gui")
            (patch_opt, "Load a patch", cxxopts::value<std::string>())
            (package_path_opt, "Packages directory path", cxxopts::value<std::string>())
            (patch_path_opt, "Patchs directory path", cxxopts::value<std::string>())
        ;

        const auto parsed_arguments = parser.parse(argc, argv);

        if (parsed_arguments.count(patch_opt) > 0)
            options.initial_patch_path = parsed_arguments[patch_opt].as<std::string>();
        if (parsed_arguments.count(no_gui_opt) > 0)
            options.no_gui = true;

        if (parsed_arguments.count(package_path_opt) > 0)
            options.application_configuration.package_path =
                parsed_arguments[package_path_opt].as<std::string>();
        else
            options.application_configuration.package_path =
                Gammou::default_configuration::get_packages_directory_path();

        if (parsed_arguments.count(patch_path_opt) > 0)
            options.application_configuration.patch_path =
                parsed_arguments[patch_path_opt].as<std::string>();
        else
            options.application_configuration.patch_path =
                Gammou::default_configuration::get_patch_path();

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
        options.application_configuration,
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