#ifndef GAMMOU_PACKAGE_LOADER_H_
#define GAMMOU_PACKAGE_LOADER_H_

#include <filesystem>
#include <map>

#include "node_widget_factory.h"

namespace Gammou
{
    using package_uid = uint64_t;

    class node_widget_factory_builder
    {
    public:
        struct dependency
        {
            package_uid uid;
            std::string name;
        };

        struct package
        {
            package_uid uid;
            std::string name;
            std::vector<dependency> dependencies{};
            std::vector<std::unique_ptr<node_widget_factory::plugin>> loaded_plugins{};
            std::unique_ptr<llvm::Module> lib_module{};
        };

        node_widget_factory_builder(llvm::LLVMContext& llvm_context);

        /**
         *  \brief Add a package in the factory
         */
        node_widget_factory_builder& add_package(package&& package);

        /**
         *  \brief Load a package
         *  \param package_root_dir_path the root directory of the package (this directory must contain a content.json file)
         *  \details a package is a directory containing a file content.json
         */
        node_widget_factory_builder& load_package(const std::filesystem::path& package_root_dir_path);

        /**
         *  \brief Load all package located in a given directory (without recursing in subdir)
         *  \param packages_dir_path the directory in which packages will be looked for
         */
        node_widget_factory_builder& load_packages(const std::filesystem::path& packages_dir_path);

        /**
         *  \brief build a factory with the loaded packages
         *  \note Resolve packages dependency before building,
         *  and remove packages with missing dependencies
         */
        std::unique_ptr<node_widget_factory> build();

    private:

        void _resolve_dependencies();
        package _load_package(const std::filesystem::path& package_root_dir_path);

        std::map<package_uid, package> _packages{};
        llvm::LLVMContext& _llvm_context;
    };
}

#endif