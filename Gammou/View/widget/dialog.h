#ifndef VIEW_DIALOG_H_
#define VIEW_DIALOG_H_

#include <view.h>


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

        class file_explorer_dialog 
#ifndef _WIN32			
			: public dialog 
#endif
		{

        public:
            file_explorer_dialog(
                const std::string& initial_path, 
                const color background = cl_lightgrey);
            virtual ~file_explorer_dialog();

            bool get_filename(std::string& name);
#ifdef _WIN32	
			void show(const std::string& window_title = "");
#endif
            static bool path_dialog(
                std::string & path, 
                const std::string & title, 
                const std::string & ext);

        private:
            bool m_path_was_set;
            std::string m_path;
		};

    } /* View */

} /* Gammou */

#endif
