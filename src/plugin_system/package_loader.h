#ifndef GAMMOU_PACKAGE_LOADER_H_
#define GAMMOU_PACKAGE_LOADER_H_

#include <filesystem>
#include "node_widget_factory.h"

namespace Gammou
{
    /**
     *  \brief  Load a package into a node_widget_factory
     *  \param package_root_dir_path the root directory of the package (this directory must contain a content.json file)
     *  \param factory the factory in which all the plugins will be loaded
     *  \details a package is a directory containing a file content.json
     */
    void load_package(const std::filesystem::path& package_root_dir_path, node_widget_factory& factory);

    /**
     *  \brief Load all package located in a given directory (without recursing in subdir)
     *  \param packages_dir_path the directory in which packages will be looked for
     *  \param factory the factory in which all the packages will be loaded
     */
    void load_all_packages(const std::filesystem::path& packages_dir_path, node_widget_factory& factory);
}

#endif