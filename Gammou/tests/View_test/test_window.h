
#include <memory>
#include <view.h>
#include <filesystem>
#include <variant>

namespace Gammou {

    
    class test_window : public View::window_widget {

        public:
            test_window(
                const unsigned int px_width, 
                const unsigned int py_height);
    };


} /* Gammou */

