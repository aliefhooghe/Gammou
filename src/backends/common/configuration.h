#ifndef GAMMOU_CONFIGURATION_H_
#define GAMMOU_CONFIGURATION_H_

#include <filesystem>

#define GAMMOU_PACKAGE_PATH_ENV "GAMMOU_PACKAGE_PATH"

#ifndef GAMMOU_PACKAGE_PATH_DEFAULT
#define GAMMOU_PACKAGE_PATH_DEFAULT "/home/.gammou/packages"
#endif

namespace Gammou {

    /**
     *  @brief Retrive the packages directory
     *          From an environment variable or a default value
     */
    std::filesystem::path get_packages_directory_path();

}

#endif