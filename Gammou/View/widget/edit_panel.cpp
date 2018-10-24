
#include "edit_panel.h"

namespace Gammou {

    namespace View {

        /*
         *
         *      edit_widget implementation
         *
         */

        edit_widget::edit_widget(const int x, const int y, const unsigned int width, const unsigned int height)
        :   widget (x, y, width, height)
        {
        }

        edit_widget::edit_widget(const rectangle& rect)
        :   widget(rect)
        {
        }

        void edit_widget::draw(cairo_t *cr)
        {
            cairo_helper::set_source_color(cr, is_selected() ? cl_green : cl_darkgray);
            cairo_helper::simple_rectangle(cr, get_relative_rect());
            cairo_stroke(cr);
        }

        bool edit_widget::is_selected() const
        {
            return m_is_selected;
        }

    } /* View */

} /* Gammou */
