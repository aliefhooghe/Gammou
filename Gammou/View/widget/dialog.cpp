

#ifdef _WIN32	
#include <Windows.h>
#else
#include "file_system_view.h"
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
            dialog_display display(*get_display(), *this);
			DEBUG_PRINT("Opening Dialog\n");
            display.open(window_title);
			DEBUG_PRINT("Wainting Dialog\n");
			display.wait();
			DEBUG_PRINT("Dialog was closed\n");
        }

        //--------
       
#ifndef _WIN32
		file_explorer_dialog::file_explorer_dialog(
			const std::string& root_path,
			const mode mode)
			:
			dialog(300, 400),
			m_path_was_set(false),
			m_mode(mode)
		{
			auto dir_view =
				std::make_unique<View::file_system_view>(
					root_path,
					0, 0,
					get_width(), get_height(),
					15);

			dir_view->set_value_open_event(
				[this](View::directory_view<std::string>&, const std::string&, const std::string& p)
			{
				m_path = p;
				m_path_was_set = true;
				auto * dpy = get_display();
				if (dpy != nullptr)
					dpy->close();
			});

			add_widget(std::move(dir_view));
		}      
#else
		file_explorer_dialog::file_explorer_dialog(
			const std::string& root_path,
			const mode mode)
		:	m_path_was_set(false),
			m_mode(mode),
			m_root_path(root_path)
		{

        }
#endif

        file_explorer_dialog::~file_explorer_dialog()
        {

        }

        bool file_explorer_dialog::get_filename(std::string& name)
        {
            if (m_path_was_set) {
                name = m_path;
                return true;
            }
            return false;
        }

#ifdef _WIN32	
		void file_explorer_dialog::show(const std::string& window_title)
		{
			//const std::string ext = "";	// will be used as parameter
			//const char *cext = ext.c_str();
			//const unsigned int ext_len = 
			//	static_cast<unsigned int>(ext.size());
			//char filter[256];

			//	Prepare Extension filter	:	TODO
			/*
			for (unsigned int i = 0; i < ext_len; ++i) {
				filter[i] = cext[i];
				filter[i + ext_len + 3] = cext[i];
			}

			filter[ext_len] = '\0';
			filter[ext_len + 1] = '*';
			filter[ext_len + 2] = '.';
			filter[(2 * ext_len) + 3] = '\0';
			filter[(2 * ext_len) + 4] = '\0';
			*/
			//

			OPENFILENAMEA dialog;
			char cpath[256] = "";

			std::memset(&dialog, 0, sizeof(dialog));

			dialog.lStructSize = sizeof(dialog);
			dialog.hwndOwner = nullptr;
			dialog.hInstance = nullptr;
			dialog.lpstrFilter = nullptr;
			dialog.lpstrFile = cpath;
			dialog.lpstrInitialDir = m_root_path.c_str();
			dialog.nMaxFile = 256;
			dialog.lpstrTitle = window_title.c_str();
			dialog.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

			bool ret;

			if (m_mode == mode::OPEN)
				ret = GetOpenFileNameA(&dialog);
			else
				ret = GetSaveFileNameA(&dialog);
			
			if (ret) {
				m_path = std::string(cpath);
				m_path_was_set = true;
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

            if (dialog.m_path_was_set) {
                path = dialog.m_path;
                return true;
            }

            return false;
        }

    }   /* View */

}   /* Gammou */
