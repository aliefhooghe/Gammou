
#include <cstdlib>
#include "configuration.h"

namespace Gammou {

    std::filesystem::path get_packages_directory_path()
    {
        auto env_var = std::getenv(GAMMOU_PACKAGE_PATH_ENV);

        if (env_var)
            return env_var;
        else
            return GAMMOU_PACKAGE_PATH_DEFAULT;
    }

}