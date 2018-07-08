

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
                    
                    if (View::file_explorer_dialog::path_dialog(path, "Ouvrir Fichier", "")) 
                        std::cout << "Ouvert " << path << std::endl;
                    else
                        std::cout << "Ouverture annulée" << std::endl;
                },
                "Open Explorer",
                400, 400));

		add_widget(
			std::make_unique<View::push_button>(
				[this](View::push_button *p)
				{
			get_display()->non_blocking_close();
				},
				"Close",
				600, 400));

        add_widget(std::move(list_box_ptr));
    }

} /* Gammou */