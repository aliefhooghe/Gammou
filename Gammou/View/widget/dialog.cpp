
#include "dialog.h"

namespace Gammou {

    namespace View {

        dialog::dialog(
            const unsigned int width,
            const unsigned int height,
            const color background)
        :   panel<>(0, 0, width, height)
        {

        }

        void dialog::show(
            const std::string& window_title)
        {
            dialog_display display(*this);
            display.open(window_title);
            while (display.is_open())
                std::this_thread::sleep_for(std::chrono::seconds(1));   //  TODO mieu
        }

        //--------
        
        file_explorer_dialog::file_explorer_dialog(
            const std::string& initial_path,
            const color background)
        :   dialog(300, 400, background),
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
        }

        bool file_explorer_dialog::get_filename(std::string& name)
        {
            if (m_filename_was_set) {
                name = m_filename;
                return true;
            }
            return false;
        }

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

    }   /* View */

}   /* Gammou */