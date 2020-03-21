
#include "gui/circuit_editor.h"
#include <test_implentations.h>

int main()
{
    auto create_node = [](){
        return std::make_unique<Gammou::owning_node_widget>("node",
            std::make_unique<DSPJIT::add_compile_node>());
    };

    auto editor = std::make_unique<Gammou::circuit_editor>(50, 30);
    editor->set_create_callback(create_node);

    auto window = std::make_unique<View::background>(std::move(editor));
    View::native_application_display dpy{*window, 13};
    dpy.open("");
    dpy.wait();

    std::cout << "FIN" << std::endl;


    return 0;
}