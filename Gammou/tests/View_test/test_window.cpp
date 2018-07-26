

#include <iostream>
#include "../../Application/gui/gui_properties.h"
#include "test_window.h"


namespace Gammou {

    test_window::test_window(
        const unsigned int px_width,
        const unsigned int px_height)
        : View::window_widget(px_width, px_height, View::cl_gray)
    {

        //  Label Test

        add_widget(
            std::make_unique<View::label>(
                "This is a label",
                10, 10, 100, 12,
                View::cl_black));

        //  Push Button Test

        add_widget(
            std::make_unique<View::push_button>(
                [](View::push_button *button)
                {
                    std::cout << "Button pushed !" << std::endl;
                },
                "Click Me !",
                10, 100));

        //  Knob Test

        add_widget(
            std::make_unique<View::knob>(
                [](View::knob *knob)
                {
                    std::cout << "Knob value = " <<
                    knob->get_normalized_value() << std::endl;
                },
                10, 150
            ));

        //  Slider TEst

        add_widget(
            std::make_unique<View::slider>(
                [](View::slider& slider)
                {
                    std::cout << "Slider Value = " << slider.get_normalized_value() << std::endl;
                },
                100, 150, 250));

        //  Edit Panel Test

        auto edit_panel =
            std::make_unique<View::edit_panel<> >(
                500, 150,
                300, 300);

        //  ListBox Test

        auto list_box_ptr = 
            std::make_unique<View::list_box>(
                10, 300, 
                200, 400,
                18);

        list_box_ptr->
            set_item_select_event(
                [](View::list_box&, unsigned int id){
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
                380, 300));

        add_widget(std::move(list_box_ptr));

        //  File Explorer Dialog Test

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

        //  Close Diaplay From Widget Test

		add_widget(
			std::make_unique<View::push_button>(
				[this](View::push_button *p)
				{
					std::cout << "Non blocking close\n";
					get_display()->close();
                },
				"Close",
                600, 40));

        //  Dialog Opening Test

		add_widget(
			std::make_unique<View::push_button>(
				[this](View::push_button *p)
				{
					test_dialog dialog;
					dialog.show("Dialog Title");
				},
				"Dialog",
                820, 400));



        add_widget(std::move(edit_panel));
    }

    bool test_window::on_mouse_dbl_click(const int x, const int y)
    {
        DEBUG_PRINT("Mouse DoublClick !\n");
        return true;
    }

	test_dialog::test_dialog()
		: View::dialog(100, 100, View::cl_chartreuse)
	{
		add_widget(
			std::make_unique<View::push_button>(
				[this](View::push_button *self)
				{
					get_display()->close();
				},
				"Ok",
				20, 20, 60, 60));
	}

} /* Gammou */
