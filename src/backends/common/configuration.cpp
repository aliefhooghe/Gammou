
#include <cstdlib>
#include "configuration.h"

namespace Gammou {

    std::filesystem::path get_packages_directory_path()
    {
        if (auto package_path = std::getenv(GAMMOU_PACKAGE_PATH_ENV))
            return package_path;
        else if (auto home = std::getenv("HOME"))
            return std::filesystem::path{home} / ".gammou/packages";
        else
            return {};
    }

}