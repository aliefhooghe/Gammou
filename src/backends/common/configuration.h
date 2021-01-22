#ifndef GAMMOU_CONFIGURATION_H_
#define GAMMOU_CONFIGURATION_H_

#include <filesystem>

#define GAMMOU_PACKAGE_PATH_ENV "GAMMOU_PACKAGE_PATH"
#define GAMMOU_PATCH_PATH_ENV "GAMMOU_PATCH_PATH"

namespace Gammou {


    class configuration {
    public:

        /**
         *  @brief Retrive the packages directory
         *          From an environment variable or a default value
         */
        static std::filesystem::path get_packages_directory_path();

        /**
         *
         */
        static std::filesystem::path get_patch_path();

    };

}

#endif