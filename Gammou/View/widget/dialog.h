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
			enum class mode { OPEN, SAVE };

			file_explorer_dialog(
                const std::string& initial_path, 
				const mode mode = mode::OPEN);
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
			const mode m_mode;
#ifdef _WIN32
			const std::string m_root_path;
#endif
		};




    } /* View */

} /* Gammou */

#endif
