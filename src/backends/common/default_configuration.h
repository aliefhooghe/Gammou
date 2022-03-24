#ifndef GAMMOU_CONFIGURATION_H_
#define GAMMOU_CONFIGURATION_H_

#include <filesystem>

#define GAMMOU_PACKAGE_PATH_ENV "GAMMOU_PACKAGE_PATH"
#define GAMMOU_PATCH_PATH_ENV "GAMMOU_PATCH_PATH"
#define GAMMOU_SAMPLE_PATH_ENV "GAMMOU_SAMPLE_PATH"

namespace Gammou {


    class default_configuration {
    public:

        /**
         *  @brief Retrieve the packages directory from an environment variable or a default value
         */
        static std::filesystem::path get_packages_directory_path();

        /**
         *  @brief Retrieve the patch directory from an environment variable or a default value
         */
        static std::filesystem::path get_patch_path();

        /**
         *  @brief Retrieve the sample directory from an environment variable or a default value
         */
        static std::filesystem::path get_samples_path();

    };

}

#endif