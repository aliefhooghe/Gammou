#pragma once

#include "../../debug.h"
#include "view.h"


namespace Gammou {

	namespace Vst {

		class synthesizer_gui : public View::generic_window {

		public:
			synthesizer_gui(const unsigned int width, const unsigned int height) 
				: View::generic_window(width, height), movable(true)
			{
				set_background_color(View::cl_white);

				square = new View::movable_control(200, 100, 66, 66);	
				add_widget(square);


				add_widget(new View::push_button([&] { set_background_color(View::create_color(0.78f, 0.95f, 0.86f)); }, "BOUTON", 100, 50));
				add_widget(new View::push_button([&] { set_background_color(View::create_color(0.95f, 0.78f, 0.86f)); }, "BOUTON", 150, 100));
				add_widget(new View::push_button([&] { set_background_color(View::create_color(0.78f, 0.86f, 0.95f)); }, "BOUTON", 200, 150));
				add_widget(new View::push_button([&] { set_background_color(View::create_color(0.95f, 0.86f, 0.78f)); }, "BOUTON", 250, 200));
				add_widget(new View::push_button([&] { set_background_color(View::create_color(0.86f, 0.95f, 0.78f)); }, "BOUTON", 300, 250));
				add_widget(new View::push_button([&] { set_background_color(View::create_color(0.86f, 0.78f, 0.95f)); }, "BOUTON", 350, 300));


				add_widget(new View::push_button([&] { set_background_color(View::create_color(0.45f, 0.45f, 0.02f)); }, "Clement", 10, 200, 70, 70, 15));
			}
			
			~synthesizer_gui() 
			{
			}

		protected:
			bool on_mouse_dbl_click(const int x, const int y) override
			{
				DEBUG_PRINT("DBLCLICK\n");
				return true;
			}

		private:
			bool movable;
			View::movable_control *square;
		};
	}
}
