

#include <iostream>
#include "../../Application/gui/gui_properties.h"
#include "test_window.h"


namespace Gammou {

    test_window::test_window(
        const unsigned int px_width,
        const unsigned int px_height)
        : View::window_widget(px_width, px_height, View::cl_gray)
    {

        add_widget(
            std::make_unique<View::label>(
                "This is a label",
                10, 10, 100, 12,
                View::cl_black));

        add_widget(
            std::make_unique<View::push_button>(
                [](View::push_button *button)
                {
                    std::cout << "Button pushed !" << std::endl;
                },
                "Click Me !",
                10, 100));

        add_widget(
            std::make_unique<View::knob>(
                [](View::knob *knob)
                {
                    std::cout << "Knob value = " <<
                    knob->get_normalized_value() << std::endl;
                },
                10, 150
            ));

        auto list_box_ptr = 
            std::make_unique<View::list_box>(
                10, 300, 
                200, 400,
                18,
                [](unsigned int id){
                    std::cout << "Selected item n° " << id << std::endl;
                });

        auto& list_box = *list_box_ptr;

        add_widget(
            std::make_unique<View::push_button>(
                [&list_box](View::push_button *button)
                {
                    const unsigned int item_count = 
                        list_box.get_item_count();
                    list_box.add_item("item-" + std::to_string(item_count));
                },
                "Add Item !",
                250, 300));
        
        add_widget(
            std::make_unique<View::push_button>(
                [&list_box](View::push_button *button)
                {
                    list_box.clear();
                },
                "Clear !",
                320, 300));

        add_widget(
            std::make_unique<View::push_button>(
                [](View::push_button *p)
                {
                    std::string path;
                    
                    if (file_explorer_widget::path_dialog(path, "Ouvrir Fichier", "")) 
                        std::cout << "Ouvert " << path << std::endl;
                    else
                        std::cout << "Ouverture annulée" << std::endl;
                },
                "Open Explorer",
                400, 400));

        add_widget(std::move(list_box_ptr));
    }

    ///

    file_explorer_widget::file_explorer_widget(const std::string& initial_path)
        :   View::panel<>(0, 0, 300, 400, Gammou::Gui::GuiProperties::background),
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
                       std::cout << "selected " << m_current_path << std::endl;
                       update_list_box();       
                   }
                },
                Gammou::Gui::GuiProperties::main_gui_list_box_selected_item_color, // select
				Gammou::Gui::GuiProperties::main_gui_list_box_background,  // back
				Gammou::Gui::GuiProperties::main_gui_list_box_border_color,     // border
				Gammou::Gui::GuiProperties::main_gui_list_box_font_color,     // font
				11,  // font
				2.0f); // border
                
        m_list_box = list_box.get();
        add_widget(std::move(list_box));
        
        add_widget(
            std::make_unique<View::push_button>(
                [this](View::push_button *b)
                {
                    m_current_path = m_current_path.parent_path();
                    std::cout << "selected " << m_current_path << std::endl;
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

    bool file_explorer_widget::get_filename(std::string& name)
    {
        if (m_filename_was_set) {
            name = m_filename;
            return true;
        }
        return false;
    }

    bool file_explorer_widget::path_dialog(
                std::string & path, 
                const std::string & title, 
                const std::string & ext)
    {
        auto dialog = 
            show_dialog<file_explorer_widget>(
                        title,
                        "/home/liefhooghe");

        if (dialog->m_filename_was_set) {
            path = dialog->m_filename;
            return true;
        }

        return false;
    }

    std::filesystem::path file_explorer_widget::get_path_by_id(const unsigned int id)
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

    void file_explorer_widget::update_list_box()
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


} /* Gammou */