
#include <cstdlib>
#include "configuration.h"

namespace Gammou {

    static std::filesystem::path get_config_dir(const char *env_var, const char *home_suffix, const char *default_dir)
    {
        if (auto raw_path = std::getenv(env_var)) {
            const auto package_path = std::filesystem::path{raw_path};
            if (std::filesystem::is_directory(package_path))
                return package_path;
        }
        else if (auto home = std::getenv("HOME")) {
            const auto package_path = std::filesystem::path{home} / home_suffix;
            if (std::filesystem::is_directory(package_path))
                return package_path;
        }

        // default
        return default_dir;
    }

    std::filesystem::path configuration::get_packages_directory_path()
    {
        return get_config_dir(GAMMOU_PACKAGE_PATH_ENV, ".gammou/packages", "./packages");
    }

    std::filesystem::path configuration::get_patch_path()
    {
        return get_config_dir(GAMMOU_PATCH_PATH_ENV, ".gammou/patchs", "./patchs");
    }

    std::filesystem::path configuration::get_samples_path()
    {
        return get_config_dir(GAMMOU_SAMPLE_PATH_ENV, ".gammou/samples", "./samples");
    }

}