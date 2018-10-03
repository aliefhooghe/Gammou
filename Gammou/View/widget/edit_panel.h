#ifndef EDIT_PANEL_H_
#define EDIT_PANEL_H_

#include <list>
#include "scrollable_panel.h"

namespace Gammou {

    namespace View {

        class item_widget : public widget {

            public:
                virtual void on_select() {}
                virtual void on_unselect() {}

                virtual void on_move_start() {}
                virtual void on_move() {}
                virtual void on_move_end() {}
        };

        template<class widget_type = item_widget>    //  MAybe special widgets ? (with set_selected, ...) OUI mais plus tard
        class edit_panel : public scrollable_panel<widget_type> {

            public:
                enum class control_mode {DEFAULT, SELECT, MOVE};

                edit_panel(const int x, const int y,
                           const unsigned int width,
                           const unsigned int height,
                           const color background = cl_white);
                edit_panel(const rectangle& rect, const color background = cl_white);
                virtual ~edit_panel();

            /*
             *
             *      Mouse Usages:
             *          -   default     :   scrolable_panel<> methods
             *          -   select      :   call on_select_event
             *          -   Move        :   move the selection
             *
             */

                virtual bool on_mouse_button_up(
                        const mouse_button button,
                        const int x, const int y) override;

                virtual bool on_mouse_drag_start(
                    const mouse_button button,
                        const int x, const int y) override;

                virtual bool on_mouse_drag(
                    const mouse_button button,
                    const int x, const int y,
                    const int dx, const int dy) override;

                virtual bool on_mouse_drag_end(
                    const mouse_button button,
                    const int x, const int y) override;

                virtual void draw(cairo_t *cr) override;

             private:

                //  Selecting
                void update_selection();

                int m_selection_start_x;
                int m_selection_start_y;
                int m_selection_end_x;
                int m_selection_end_y;
                bool m_is_selecting{false};
                std::list<widget_type*> m_selection;
                //---

                control_mode m_control_mode{control_mode::DEFAULT};
        };

        /*
         *
         *      edit_panel implementation
         *
         */

        template<class widget_type>
        edit_panel<widget_type>::edit_panel(
            const int x, const int y,
            const unsigned int width,
            const unsigned int height,
            const color background)
        :   scrollable_panel<widget_type>(
                x, y, width,
                height, background)
        {
            m_control_mode = control_mode::SELECT;
        }

        template<class widget_type>
        edit_panel<widget_type>::edit_panel(
            const rectangle& rect, const color background)
        :   scrollable_panel<widget_type>(rect, background)
        {
        }

        template<class widget_type>
        edit_panel<widget_type>::~edit_panel()
        {
        }

        template<class widget_type>
        void edit_panel<widget_type>::draw(cairo_t *cr)
        {
            scrollable_panel<widget_type>::draw(cr);

            if (m_control_mode == control_mode::SELECT &&
                    m_is_selecting) {
                double dash_width = 5.0;
                cairo_set_dash (cr, &dash_width, 1, 0);

                cairo_set_line_width(cr, 1.0);
                cairo_helper::set_source_color(cr, cl_gray);
                cairo_rectangle(
                    cr, m_selection_start_x, m_selection_start_y,
                    m_selection_end_x - m_selection_start_x,
                    m_selection_end_y - m_selection_start_y);
                cairo_stroke(cr);
            }

        }

        template<class widget_type>
        bool edit_panel<widget_type>::on_mouse_button_up(
                const mouse_button button,
                const int x, const int y)
        {
            switch (m_control_mode) {

                case control_mode::SELECT:
                    if (m_is_selecting) {
                        m_is_selecting = false;
                        m_selection.clear();
                        scrollable_panel<widget_type>::redraw();
                    }
                    return true;
                    break;

                case control_mode::MOVE:
                    return false;
                    break;

                case control_mode::DEFAULT:
                    return scrollable_panel<widget_type>::on_mouse_button_up(button, x, y);
                    break;

            }

            return true;
        }

        template<class widget_type>
        bool edit_panel<widget_type>::on_mouse_drag_start(
            const mouse_button button, const int x, const int y)
        {
            switch (m_control_mode) {

                case control_mode::DEFAULT:
                    return scrollable_panel<widget_type>::on_mouse_drag_start(button, x, y);
                    break;

                case control_mode::MOVE:
                {
                    widget_type *w = panel<widget_type>::get_focused_widget();
                    if (w != nullptr) {
                        w->set_pos(x, y);
                        w->on_move_start();
                    }
                    return true;
                }
                    break;

                case control_mode::SELECT:
                    m_selection.clear();
                    m_selection_start_x = x;
                    m_selection_start_y = y;
                    m_selection_end_x = x;
                    m_selection_end_y = y;
                    m_is_selecting = true;
                    scrollable_panel<widget_type>::redraw();
                    return true;
                    break;

            }

            return true;
        }

        template<class widget_type>
        bool edit_panel<widget_type>::on_mouse_drag(
            const mouse_button button,
            const int x, const int y,
            const int dx, const int dy)
        {

            switch (m_control_mode) {

                case control_mode::DEFAULT:
                    return scrollable_panel<widget_type>::on_mouse_drag(button, x, y, dx, dy);
                    break;

                case control_mode::MOVE:
                {
                    widget_type *w = panel<widget_type>::get_focused_widget();
                    if (w != nullptr) {
                        w->set_pos(x, y);
                        w->on_move();
                    }
                    return true;
                }
                    break;

                case control_mode::SELECT:
                {
                    m_selection_end_x = x;
                    m_selection_end_y = y;
                    update_selection();
                    scrollable_panel<widget_type>::redraw();
                    return true;
                }
                    break;

            }

            edit_panel<widget_type>::redraw();
            return true;
        }

        template<class widget_type>
        bool edit_panel<widget_type>::on_mouse_drag_end(
            const mouse_button button,
            const int x, const int y)
        {
            switch (m_control_mode) {

                case control_mode::DEFAULT:
                    return scrollable_panel<widget_type>::on_mouse_drag_end(button, x, y);
                    break;

                case control_mode::MOVE:
                {
                    widget_type *w = panel<widget_type>::get_focused_widget();
                    if (w != nullptr) {
                        w->set_pos(x, y);
                        w->on_move_end();
                    }
                    return true;
                }
                    break;

                case control_mode::SELECT:
                    //  TODO
                    return true;
                    break;

            }

            return true; // for compiler
        }

        template<class widget_type>
        void edit_panel<widget_type>::update_selection()
        {/*
            const rectangle select_area =
            {
                .x = std::min(m_selection_start_x, m_selection_end_x),
                .y = std::min(m_selection_start_y, m_selection_end_y),
                .width = abs(m_selection_start_x - m_selection_end_x),
                .height = abs(m_selection_start_y - m_selection_end_y),
            };

            m_selection.clear();

            for (auto& w : panel<widget_type>::m_widgets) {
                if (select_area.overlap(w->get_absolute_rect()))
                    m_selection.push_back(w.get());
            }*/
        }


    } /* View */

} /* Gammou */

#endif
