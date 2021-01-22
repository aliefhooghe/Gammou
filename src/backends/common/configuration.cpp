
#include <cstdlib>
#include "configuration.h"

namespace Gammou {

    std::filesystem::path configuration::get_packages_directory_path()
    {
        if (auto package_path = std::getenv(GAMMOU_PACKAGE_PATH_ENV))
            return package_path;
        else if (auto home = std::getenv("HOME"))
            return std::filesystem::path{home} / ".gammou/packages";
        else
            return "./packages";
    }

    std::filesystem::path configuration::get_patch_path()
    {
        if (auto package_path = std::getenv(GAMMOU_PATCH_PATH_ENV))
            return package_path;
        else if (auto home = std::getenv("HOME"))
            return std::filesystem::path{home} / ".gammou/patchs";
        else
            return "./patchs";
    }

}