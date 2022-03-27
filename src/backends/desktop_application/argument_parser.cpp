
#include <cxxopts.hpp>
#include <string>
#include <DSPJIT/log.h>

#include "backends/common/default_configuration.h"
#include "argument_parser.h"

namespace Gammou
{
    static constexpr auto no_gui_opt_key = "no-gui";
    static constexpr auto patch_opt_key = "patch";
    static constexpr auto package_path_opt = "packages-path";
    static constexpr auto patch_path_opt_key = "patchs-path";

    static void fill_options(const cxxopts::ParseResult& parsed_arguments, application_options& options)
    {
        if (parsed_arguments.count(patch_opt_key) > 0)
            options.configuration.initial_path = parsed_arguments[patch_opt_key].as<std::string>();
        if (parsed_arguments.count(no_gui_opt_key) > 0)
            options.no_gui = true;

        if (parsed_arguments.count(package_path_opt) > 0)
            options.configuration.application_config.packages_path =
                parsed_arguments[package_path_opt].as<std::string>();
        else
            options.configuration.application_config.packages_path =
                Gammou::default_configuration::get_packages_directory_path();

        if (parsed_arguments.count(patch_path_opt_key) > 0)
            options.configuration.application_config.patchs_path =
                parsed_arguments[patch_path_opt_key].as<std::string>();
        else
            options.configuration.application_config.patchs_path =
                Gammou::default_configuration::get_patch_path();
    }

    bool parse_options(int argc, char **argv, application_options& options)
    {
        // Parse command line options
        cxxopts::Options parser("Gammou", "Gammou command line interface");

        parser.add_options()
            (no_gui_opt_key, "Run without gui")
            (patch_opt_key, "Load a patch", cxxopts::value<std::string>())
            (package_path_opt, "Packages directory path", cxxopts::value<std::string>())
            (patch_path_opt_key, "Patchs directory path", cxxopts::value<std::string>())
            ("h,help", "Print help")
        ;

        try {
            const auto parsed_arguments = parser.parse(argc, argv);
            if (!parsed_arguments.count("help"))
            {
                fill_options(parsed_arguments, options);
                return true;
            }
        }
        catch (std::exception& error)
        {
            LOG_WARNING("%s\n", error.what());
        }

        LOG_INFO("%s\n", parser.help().c_str());
        return false;
    }

}