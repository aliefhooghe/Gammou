
#include <memory>
#include <view.h>
#include <filesystem>

namespace Gammou {

    typedef View::application_display dialog_display;

    template<class DialogType, class ...args>
    std::unique_ptr<DialogType> show_dialog(const std::string& title, args ...params)
    {
        auto dialog = std::make_unique<DialogType>(params...);
        dialog_display display(*dialog);
        dialog->set_display(&display);
        display.open(title);
        while(display.is_open())
            std::this_thread::sleep_for(std::chrono::seconds(1));
        return std::move(dialog);
    };
    
    class test_window : public View::window_widget {

        public:
            test_window(
                const unsigned int px_width, 
                const unsigned int py_height);
    };

    class dialog_widget : public View::panel<> {

        public:
            dialog_widget(
                const unsigned int width, 
                const unsigned int height,
                const View::color background);
            virtual ~dialog_widget() {}
            void set_display(dialog_display *display);

        protected:
            void close_display();
        
        private:
            dialog_display *m_display{};
    };

    class file_explorer_widget : public dialog_widget {

        public:
            file_explorer_widget(const std::string& initial_path);
            ~file_explorer_widget() {std::cout << "FileDialog dtor\n";}            

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

    

} /* Gammou */


