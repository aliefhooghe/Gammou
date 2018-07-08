

#ifdef _WIN32	
#include <Windows.h>
#endif

#include "dialog.h"

namespace Gammou {

    namespace View {

        dialog::dialog(
            const unsigned int width,
            const unsigned int height,
            const color background)
        :   panel<>(0, 0, width, height, background)
        {

        }

        void dialog::show(
            const std::string& window_title)
        {
            dialog_display display(*this);
			DEBUG_PRINT("Opening Dialog\n");
            display.open(window_title);
			DEBUG_PRINT("Wainting Dialog\n");
			while (display.is_open());
              //  std::this_thread::sleep_for(std::chrono::seconds(1));   //  TODO mieu
			DEBUG_PRINT("Dialog was closed\n");
        }

        //--------
        
        file_explorer_dialog::file_explorer_dialog(
            const std::string& initial_path,
            const color background)
			:
#ifndef _WIN32
			dialog(300, 400, background),
            m_filename_was_set(false),
            m_current_path(initial_path)
        {
            auto list_box = 
                std::make_unique<View::list_box>(
                    0, 0, 
                    get_width(), get_height() - 30,
                    15,
                    [this](unsigned int id){
                    auto p = get_path_by_id(id);
                    if (std::filesystem::is_directory(p)) {
                        m_current_path = std::move(p);
                        update_list_box();       
                    }
                    });
                
            m_list_box = list_box.get();
            add_widget(std::move(list_box));
            
            add_widget(
                std::make_unique<View::push_button>(
                    [this](View::push_button *b)
                    {
                        m_current_path = m_current_path.parent_path();
                        update_list_box();
                    },
                    "up",
                    1,
                    get_height() - 30 + 1,
                    get_width() / 2 - 2,
                    30 - 2));

            add_widget(
                std::make_unique<View::push_button>(
                    [this](View::push_button *b)
                    {
                        const auto selected_item = 
                            m_list_box->get_selected_item();
                        
                        if (selected_item < 0)
                            return;

                        auto p = get_path_by_id(selected_item);
                        m_filename = p;
                        m_filename_was_set = true;
                        get_display()->non_blocking_close();
                    },
                    "Open",
                    1 + get_width() / 2,
                    1 + get_height() - 30,
                    get_width() / 2 - 2,
                    30 - 2));


            update_list_box();
#else
			m_filename_was_set(false)
		{
#endif
        }

        bool file_explorer_dialog::get_filename(std::string& name)
        {
            if (m_filename_was_set) {
                name = m_filename;
                return true;
            }
            return false;
        }

#ifdef _WIN32	
		void file_explorer_dialog::show(const std::string& window_title)
		{
			const std::string ext = "";	// will be used as parameter
			OPENFILENAMEA dialog;

			const char *cext = ext.c_str();
			const unsigned int ext_len = static_cast<unsigned int>(ext.size());
			char cpath[256] = ""; // ..[0] = '\0'
			char filter[256];

			// 
			for (unsigned int i = 0; i < ext_len; ++i) {
				filter[i] = cext[i];
				filter[i + ext_len + 3] = cext[i];
			}

			filter[ext_len] = '\0';
			filter[ext_len + 1] = '*';
			filter[ext_len + 2] = '.';
			filter[(2 * ext_len) + 3] = '\0';
			filter[(2 * ext_len) + 4] = '\0';

			std::memset(&dialog, 0, sizeof(dialog));

			dialog.lStructSize = sizeof(dialog);
			dialog.hwndOwner = nullptr;
			dialog.hInstance = nullptr; //
			dialog.lpstrFilter = filter;
			dialog.lpstrFile = cpath;
			dialog.nMaxFile = 256;
			dialog.lpstrTitle = window_title.c_str();
			dialog.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

			if (GetOpenFileNameA(&dialog)) {
				m_filename = std::string(cpath);
				m_filename_was_set = true;
			}

		}
#endif

        bool file_explorer_dialog::path_dialog(
                    std::string & path, 
                    const std::string & title, 
                    const std::string & ext)
        {
            file_explorer_dialog dialog("/");
            dialog.show(title);

            if (dialog.m_filename_was_set) {
                path = dialog.m_filename;
                return true;
            }

            return false;
        }
#ifndef _WIN32
        std::filesystem::path file_explorer_dialog::get_path_by_id(const unsigned int id)
        {
            unsigned int i = 0;
            for (auto & p : std::filesystem::directory_iterator(m_current_path)) {
                if (i == id) {
                    return std::move(p);
                }
                i++;
            }

            throw std::runtime_error("id invalide get_path_by_id");
        }

        void file_explorer_dialog::update_list_box()
        {
            m_list_box->clear();

            for (auto & p : std::filesystem::directory_iterator(m_current_path)) {
                const std::string entry{p.path().filename()};

                if (std::filesystem::is_directory(p))
                    m_list_box->add_item(entry + "/");
                else
                    m_list_box->add_item(entry);

            }
        }
#endif
    }   /* View */

}   /* Gammou */