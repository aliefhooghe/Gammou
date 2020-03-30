
#include <fstream>
#include <nlohmann/json.hpp>

#include "package_loader.h"

namespace Gammou {

    struct package_descriptor {
        std::string package_name{};
        std::vector<node_widget_external_plugin_descriptor> node_class{};
        //std::vector<std::string> additional_modules{};
    };

    void from_json(const nlohmann::json& j, package_descriptor& desc)
    {
        j.at("package-name").get_to(desc.package_name);
        j.at("node-class").get_to(desc.node_class);
        //j.at("additional_modules").get_to(desc.additional_modules);
    }

    void load_package(const std::filesystem::path& dir_path, node_widget_factory& factory)
    {
        using namespace std::filesystem;
        LOG_DEBUG("[gammou][load package] Scanning package '%s'\n", dir_path.c_str());

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

        LOG_INFO("[gammou][load package] Loading package '%s'\n", dir_path.c_str());

        //  Load nodes classes to factory
        for (auto& node_class_desc : package_desc.node_class) {

            //  Make sure that path are according to pwd
            for (auto& path : node_class_desc.modules_paths) {
                if (path.is_relative())
                    path = dir_path / path;
            }

            //  Create and plugin into factory
            LOG_INFO("[gammou][load package] Loading plugin uid : 0x%016lx, name : '%s'\n", node_class_desc.uid, node_class_desc.name.c_str());

            try {
                auto plugin = std::make_unique<node_widget_external_plugin>(factory.get_llvm_context(), node_class_desc);
                factory.register_plugin(node_class_desc.uid, std::move(plugin));
            }
            catch (...)
            {
                LOG_ERROR("[gammou][load package] Failed to load plugin '%s'\n", node_class_desc.name.c_str());
            }
        }
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
        catch(...)
        {
            LOG_ERROR("[gammou][load all packages] Unable to open directory package '%s'.\n", packages_dir_path.c_str());
        }
    }
}