
#include <cstdlib>
#include "configuration.h"

namespace Gammou {

    std::filesystem::path configuration::get_packages_directory_path()
    {
        if (auto raw_path = std::getenv(GAMMOU_PACKAGE_PATH_ENV)) {
            const auto package_path = std::filesystem::path{raw_path};
            if (std::filesystem::is_directory(package_path))
                return package_path;
        }
        else if (auto home = std::getenv("HOME")) {
            const auto package_path = std::filesystem::path{home} / ".gammou/packages";
            if (std::filesystem::is_directory(package_path))
                return package_path;
        }

        // default
        return "./packages";
    }

    std::filesystem::path configuration::get_patch_path()
    {
        if (auto raw_path = std::getenv(GAMMOU_PATCH_PATH_ENV)) {
            const auto patch_path = std::filesystem::path{raw_path};
            if (std::filesystem::is_directory(raw_path))
                return raw_path;
        }
        else if (auto home = std::getenv("HOME")) {
            const auto patch_path = std::filesystem::path{home} / ".gammou/patchs";
            if (std::filesystem::is_directory(patch_path))
                return patch_path;
        }

        // default
        return "./patchs";
    }

}