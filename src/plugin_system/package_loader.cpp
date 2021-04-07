
#include <fstream>
#include <nlohmann/json.hpp>
#include <llvm/IRReader/IRReader.h>
#include <llvm/Support/SourceMgr.h>

#include "package_loader.h"
#include "node_widget_external_plugin.h"

namespace Gammou {

    using package_uid = uint64_t;

    struct package_descriptor {
        std::string package_name{};
        package_uid uid;
        std::vector<node_widget_external_plugin_descriptor> plugins{};
        std::vector<std::filesystem::path> common_libs{};
    };

    void from_json(const nlohmann::json& j, package_descriptor& desc)
    {
        j.at("package-name").get_to(desc.package_name);
        j.at("package-uid").get_to(desc.uid);

        auto it = j.find("plugins");
        if (it != j.end())
            it->get_to(desc.plugins);

        it = j.find("common-libs");
        if (it != j.end())
            it->get_to(desc.common_libs);
    }

    void load_package(const std::filesystem::path& dir_path, node_widget_factory& factory)
    {
        using namespace std::filesystem;
        LOG_DEBUG("[gammou][load package] Scanning package '%s'\n", dir_path.c_str());

        // Loaded plugin list
        std::vector<std::unique_ptr<node_widget_external_plugin>> loaded_plugins{};
        std::vector<std::unique_ptr<llvm::Module>> additionalModules{};

        //  a package is a directory
        if (!is_directory(dir_path))
            throw std::runtime_error("load_package : given path is not a directory");

        //  compute content file path
        auto content_file_path = dir_path / "content.json";
        nlohmann::json json_object;

        //  load json object from the file
        std::ifstream stream{content_file_path};
        stream >> json_object;

        //  deserialize package descriptor from the file
        auto package_desc = json_object.get<package_descriptor>();

        LOG_INFO("[gammou][load package] Loading package uid : 0x%016lx, name : '%s'\n",
            package_desc.uid,
            package_desc.package_name.c_str());

        //  Load nodes plugins to factory
        for (auto& node_class_desc : package_desc.plugins) {

            //  Make sure that path are according to pwd
            for (auto& path : node_class_desc.modules_paths) {
                if (path.is_relative())
                    path = dir_path / path;
            }

            //  Create and plugin into factory
            LOG_INFO("[gammou][load package] Loading plugin uid : 0x%016lx, name : '%s'\n", node_class_desc.plugin_id, node_class_desc.name.c_str());

            try {
                loaded_plugins.emplace_back(
                    std::make_unique<node_widget_external_plugin>(factory.get_llvm_context(), node_class_desc));
            }
            catch (...)
            {
                LOG_ERROR("[gammou][load package] Failed to load plugin '%s'\n", node_class_desc.name.c_str());
            }
        }

        //  Load additional libs
        for (auto& lib_path : package_desc.common_libs) {
            llvm::SMDiagnostic error;

            if (lib_path.is_relative())
                lib_path = dir_path / lib_path;

            LOG_INFO("[gammou][load package] Loading common lib object %s\n", lib_path.c_str());
            auto module = llvm::parseIRFile(lib_path.c_str(), error, factory.get_llvm_context());

            if (!module) {
                LOG_ERROR("[gammou][load package] Cannot load common lib object %s\n", lib_path.c_str());
                throw std::runtime_error("DSPJIT : Failed to load object");
            }
            else {
                //  Strip all function attribute as they prevent inlining.
                /** \todo invesigate why ! */
                for (auto& function : *module)
                    function.setAttributes(
                        llvm::AttributeList::get(factory.get_llvm_context(), llvm::ArrayRef<llvm::AttributeList>{}));
                additionalModules.emplace_back(std::move(module));
            }
        }

        // Load plugins into the factory and additional modules into factory
        for (auto&& plugin : loaded_plugins)
            factory.register_plugin(std::move(plugin));
        for (auto&& module : additionalModules)
            factory.add_module(std::move(module));
    }

    void load_all_packages(const std::filesystem::path& packages_dir_path, node_widget_factory& factory)
    {
        LOG_DEBUG("[gammou][load package] Listing directory '%s'\n", packages_dir_path.c_str());

        try {
            for (const auto& entry : std::filesystem::directory_iterator(packages_dir_path)) {
                if (std::filesystem::is_directory(entry)) {
                    try {
                        load_package(entry.path(), factory);
                    }
                    catch(...)
                    {
                        LOG_WARNING("[gammou][load all packages] Unable to load package '%s'.\n", entry.path().c_str());
                    }
                }
            }
        }
        catch(const std::exception& e)
        {
            LOG_ERROR("[gammou][load all packages] Unable to open directory package '%s' (%s).\n", packages_dir_path.c_str(), e.what());
        }
        catch(...)
        {
            LOG_ERROR("[gammou][load all packages] Unable to open directory package '%s'.\n", packages_dir_path.c_str());
        }
    }
}