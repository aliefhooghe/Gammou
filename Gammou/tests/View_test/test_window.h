
#include <memory>
#include <view.h>
#include <variant>

namespace Gammou {

    
    class test_window : public View::window_widget {

        public:
            test_window(
                const unsigned int px_width, 
                const unsigned int py_height);

            bool on_mouse_dbl_click(const int x, const int y) override;
    };

	class test_dialog : public View::dialog {

		public:
			test_dialog();

	};


} /* Gammou */


