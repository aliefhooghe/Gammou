#ifndef FILE_SYSTEM_DIRECTORY_MODEL_H
#define FILE_SYSTEM_DIRECTORY_MODEL_H

#include <string>
#include <filesystem>
#include "directory_model.h"

#include "../../debug.h"

namespace Gammou {

    namespace View {

        class file_system_directory_model :
            public storage_directory_model<std::string, std::string> {

            public:
                using typename directory_model<std::string, std::string>::item;
                using typename directory_model<std::string, std::string>::node;
                using typename directory_model<std::string, std::string>::directory;

                file_system_directory_model();
                file_system_directory_model(const std::string& root_path);
                file_system_directory_model(const std::filesystem::path& root_path);
                ~file_system_directory_model();

                unsigned int get_item_count() override;
                item* get_item(const std::string& key) override;
                node& get_node(const unsigned int index) override;
                bool contains(const std::string& k) override;

            protected:
                void discover();

            private:
                const std::filesystem::path m_root_path;
                bool m_discovered;
        };

    } /* View */

} /* Gammou */


#endif


