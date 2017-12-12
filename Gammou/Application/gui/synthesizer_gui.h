#pragma once

#include "../../debug.h"
#include "view.h"


namespace Gammou {

	namespace Vst {

		class GammouPluginWindow : public View::generic_window {

		public:
			GammouPluginWindow() 
				: View::generic_window(800, 400), movable(true)
			{
				set_background_color(0.78, 0.83, 0.86);

				square = new View::movable_control(200, 100, 66, 66);	
				add_widget(square);


				add_widget(new View::push_button([&] { set_background_color(0.78, 0.95, 0.86); }, "BOUTON", 100, 50));
				add_widget(new View::push_button([&] { set_background_color(0.95, 0.78, 0.86); }, "BOUTON", 150, 100));
				add_widget(new View::push_button([&] { set_background_color(0.78, 0.86, 0.95); }, "BOUTON", 200, 150));
				add_widget(new View::push_button([&] { set_background_color(0.95, 0.86, 0.78); }, "BOUTON", 250, 200));
				add_widget(new View::push_button([&] { set_background_color(0.86, 0.95, 0.78); }, "BOUTON", 300, 250));
				add_widget(new View::push_button([&] { set_background_color(0.86, 0.78, 0.95); }, "BOUTON", 350, 300));


				add_widget(new View::push_button([&] { set_background_color(0.45, 0.45, 0.02); }, "Clement", 10, 200, 70, 70, 15));
			}
			
			~GammouPluginWindow() 
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
