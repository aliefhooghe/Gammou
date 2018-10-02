
#include "file_system_directory_model.h"

namespace Gammou {

	namespace View {

		file_system_directory_model::file_system_directory_model()
        :	m_root_path(std::filesystem::current_path().root_path()), // system's root path
            m_discovered(false)
		{

		}

		file_system_directory_model::file_system_directory_model(
				const std::string& root_path)
        :	m_root_path(root_path),
            m_discovered(false)
		{

		}

		file_system_directory_model::file_system_directory_model(
				const std::filesystem::path& root_path)
        :	m_root_path(root_path),
            m_discovered(false)
		{

		}

		file_system_directory_model::~file_system_directory_model()
		{
		}

        unsigned int file_system_directory_model::get_item_count()
        {
            if (!m_discovered)
                discover();

            return storage_directory_model<std::string, std::string>::get_item_count();
        }

        file_system_directory_model::item* file_system_directory_model::get_item(const std::string& key)
		{
			if (!m_discovered)
				discover();

			return storage_directory_model<std::string, std::string>::get_item(key);
		}

        file_system_directory_model::node& file_system_directory_model::get_node(const unsigned int index)
		{
			if (!m_discovered)
				discover();

			return storage_directory_model<std::string, std::string>::get_node(index);
		}

        bool file_system_directory_model::contains(const std::string& k)
		{
			if (!m_discovered)
				discover();

			return storage_directory_model<std::string, std::string>::contains(k);
		}

		void file_system_directory_model::discover()
		{
			//	Protect in a try block to avoid throwing exception when protected folder is oppened

            DEBUG_PRINT("Discovering\n");

			try {
                for (auto& p : std::filesystem::directory_iterator(m_root_path)) {
					const std::string entry{p.path().filename()};

					if (std::filesystem::is_directory(p)) {
                        auto dir =
							std::make_unique<file_system_directory_model>(p);
                        add_directory(entry, std::move(dir));
					}
					else {
                        if (entry[0] != '.')
                            add_value(entry, p.path());
					}
				}
			}
			catch(std::filesystem::filesystem_error& error)  //  Not a fatal error
			{

			}

			m_discovered = true;
		}



	} /* View */

} /* Gammou */
