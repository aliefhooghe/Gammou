

#include <iostream>
#include "../../Application/gui/gui_properties.h"
#include "widget/cairo_helper.h"
#include "test_window.h"


namespace Gammou {

    test_window::test_window(
        const unsigned int px_width,
        const unsigned int px_height)
        : View::window_widget(px_width, px_height, View::cl_gray)
    {

        ///////////////////////////////////////////////////////////////////////
        ///////////////////////////////////////////////////////////////////////
        //  Label Test

        add_widget(
            std::make_unique<View::label>(
                "This is a label",
                10, 10, 100, 12,
                View::cl_black));

        ///////////////////////////////////////////////////////////////////////
        ///////////////////////////////////////////////////////////////////////
        //  Push Button Test

        add_widget(
            std::make_unique<View::push_button>(
                [](View::push_button *button)
                {
                    std::cout << "Button pushed !" << std::endl;
                },
                "Click Me !",
                10, 100));

        ///////////////////////////////////////////////////////////////////////
        ///////////////////////////////////////////////////////////////////////
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

        ///////////////////////////////////////////////////////////////////////
        ///////////////////////////////////////////////////////////////////////
        //  Slider TEst

        auto slider =
            std::make_unique<View::slider>(
                [](View::slider& s)
                {
                    const float f = s.get_normalized_value();
                    std::cout << "Slider Value = " << f << std::endl;
                },
                100, 150, 250);

        add_widget(std::move(slider));

        ///////////////////////////////////////////////////////////////////////
        ///////////////////////////////////////////////////////////////////////
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

        ///////////////////////////////////////////////////////////////////////
        ///////////////////////////////////////////////////////////////////////
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

        ///////////////////////////////////////////////////////////////////////
        ///////////////////////////////////////////////////////////////////////
        //  Close Display From Widget Test

		add_widget(
			std::make_unique<View::push_button>(
				[this](View::push_button *p)
				{
					std::cout << "Non blocking close\n";
					get_display()->close();
                },
				"Close",
                600, 40));

        ///////////////////////////////////////////////////////////////////////
        ///////////////////////////////////////////////////////////////////////
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

        ///////////////////////////////////////////////////////////////////////
        ///////////////////////////////////////////////////////////////////////
        //  directory view test

        auto& d1 = m_model.add_directory("Mes Documents");
        auto& d2 = m_model.add_directory("Ma Musique");

        d2.add_value("music1.mp3", 1);
        d2.add_value("music42.wav", 42);
        auto& d3 = d2.add_directory("Mozart");
        auto& d4 = d3.add_directory("Requiem");

        d3.add_value("La flute", 1);
        d4.add_value("Introitus", 5);
        d4.add_value("Lacrimosa", 2);
        d3.add_value("Don Julius", 2);

        d1.add_value("Document1" , 0);
        d1.add_value("Document2", 0);

        auto tree_view =
            std::make_unique<View::directory_view<int> >(
                m_model,
                500, 150,
                200, 400, 18);

        tree_view->set_value_select_event(
        [](View::directory_view<int>&, const std::string& key, const int& value)
        {
           DEBUG_PRINT("Selected %s, value = %d\n", key.c_str(), value);
        });

        add_widget(std::move(tree_view));
    }

    bool test_window::on_mouse_dbl_click(const int x, const int y)
    {
        DEBUG_PRINT("Mouse DoublClick !\n");
        return View::window_widget::on_mouse_dbl_click(x, y);
    }

    bool test_window::on_key_down(const View::keycode key)
    {
        if (key == View::key_A)
            DEBUG_PRINT("AAA\n");

        DEBUG_PRINT("Keyboard Key Down keycode = %u %s !\n", 
            key, key == View::key_unknown ? "(unknown)" : "");

        return View::window_widget::on_key_down(key);
    }

	test_dialog::test_dialog()
		: View::dialog(215, 256, View::cl_chartreuse)
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

	void test_dialog::draw(cairo_t *cr)
	{
		dialog::draw(cr);
		View::cairo_helper::set_source_color(cr, 0xff0000ff);
		View::cairo_helper::simple_rectangle(cr, get_relative_rect());
		cairo_stroke(cr);
	}

} /* Gammou */
