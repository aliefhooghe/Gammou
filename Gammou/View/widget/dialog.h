#ifndef VIEW_DIALOG_H_
#define VIEW_DIALOG_H_

#include <view.h>
#include <filesystem>

namespace Gammou {

    namespace View {

        class dialog : public panel<> {

            public:
                dialog(
                    const unsigned int width, 
                    const unsigned int height, 
                    const color background = cl_white);
                virtual ~dialog() {}

                virtual void show(const std::string& window_title = "");
        };

        class file_explorer_dialog : public dialog {

        public:
            file_explorer_dialog(
                const std::string& initial_path, 
                const color background = cl_lightgrey);
            virtual ~file_explorer_dialog() {}            

            bool get_filename(std::string& name);

            static bool path_dialog(
                std::string & path, 
                const std::string & title, 
                const std::string & ext);

        protected:
            std::filesystem::path get_path_by_id(const unsigned int id);

            void update_list_box();
            
            bool m_filename_was_set;
            std::string m_filename;

            std::filesystem::path m_current_path;
            View::list_box *m_list_box;
    };

    } /* View */

} /* Gammou */

#endif