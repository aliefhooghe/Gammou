#ifndef EDIT_PANEL_H_
#define EDIT_PANEL_H_

#include <list>
#include "scrollable_panel.h"

namespace Gammou {

    namespace View {

        class edit_widget : public widget {

                template<class w>
                friend class edit_panel;

            public:
                edit_widget(const int x, const int y, const unsigned int width, const unsigned int height);
                edit_widget(const rectangle& rect);
                virtual ~edit_widget() {}

                virtual void draw(cairo_t *cr) override;

                virtual void on_select() {}
                virtual void on_unselect() {}
                bool is_selected() const;

            private:
                bool m_is_selected{false};
        };

        template<class widget_type = edit_widget>    //  MAybe special widgets ? (with set_selected, ...) OUI mais plus tard
        class edit_panel : public scrollable_panel<widget_type> {

            public:

                enum class mode {
                    SCROLL_MODE,    //  scroll panel
                    EDIT_MODE       //  select and move items
                };

                edit_panel(const int x, const int y,
                           const unsigned int width,
                           const unsigned int height,
                           const color background = cl_white);
                edit_panel(const rectangle& rect, const color background = cl_white);
                virtual ~edit_panel();

                virtual bool on_mouse_button_up(
                        const mouse_button button,
                        const int x, const int y) override;

                virtual bool on_mouse_button_down(
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

                virtual void set_mode(const mode m);

             private:
                //  Selecting
                void start_select(const int x, const int y);
                void finnish_select();
                void update_selection(const int x, const int y);
                void unselect_all();

                int m_selection_start_x;
                int m_selection_start_y;
                int m_selection_end_x;
                int m_selection_end_y;
                rectangle m_selected_area;
                bool m_is_selecting{false};
                std::list<widget_type*> m_selection;

                mode m_mode{mode::SCROLL_MODE};
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
            scrollable_panel<widget_type>::set_scroll_method
                    (scrollable_panel<widget_type>::scroll_method::DRAG_SCROLL);
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

            if (m_is_selecting) {
                double dash_width = 1.0;
                cairo_set_dash (cr, &dash_width, 1, 0);

                cairo_set_line_width(cr, 1.0);
                cairo_helper::set_source_color(cr, cl_gray);
                cairo_helper::simple_rectangle(cr, m_selected_area);
                cairo_stroke(cr);
            }

        }

        template<class widget_type>
        void edit_panel<widget_type>::set_mode(const mode m)
        {
            m_mode = m;
        }

        template<class widget_type>
        bool edit_panel<widget_type>::on_mouse_button_up(
                const mouse_button button,
                const int x, const int y)
        {
            if (m_is_selecting)
                finnish_select();

            scrollable_panel<widget_type>::on_mouse_button_up(button, x, y);
            return true;
        }

        template<class widget_type>
        bool edit_panel<widget_type>::on_mouse_button_down(
                const mouse_button button,
                const int x, const int y)
        {
            if (scrollable_panel<widget_type>::get_focused_widget() == nullptr)
                unselect_all();
            scrollable_panel<widget_type>::on_mouse_button_down(button, x, y);
            return true;
        }

        template<class widget_type>
        bool edit_panel<widget_type>::on_mouse_drag_start(
            const mouse_button button, const int x, const int y)
        {
            auto *w = scrollable_panel<widget_type>::get_focused_widget();

            if (m_mode == mode::EDIT_MODE && w == nullptr) //  start selecting
                start_select(x, y);

            scrollable_panel<widget_type>::on_mouse_drag_start(button, x, y);
            return true;
        }

        template<class widget_type>
        bool edit_panel<widget_type>::on_mouse_drag(
            const mouse_button button,
            const int x, const int y,
            const int dx, const int dy)
        {
            if (m_mode == mode::EDIT_MODE) {
                if (m_is_selecting)
                    update_selection(x, y);
                else if (auto w = scrollable_panel<widget_type>::get_focused_widget()) {

                    // if w is in the current selection
                    if (w->is_selected()) {
                        for (auto& select_w : m_selection)
                            select_w->set_rect(select_w->get_absolute_rect().translate(dx, dy));
                    }
                    else {
                        // just move w
                        DEBUG_PRINT("kdjfbgjsifgjdf,gsdfgsdfg\n");
                        w->set_rect(w->get_absolute_rect().translate(dx, dy));
                    }

                }
            }
            else {
                scrollable_panel<widget_type>::on_mouse_drag(button, x, y, dx, dy);
            }

            return true;
        }

        template<class widget_type>
        bool edit_panel<widget_type>::on_mouse_drag_end(
            const mouse_button button,
            const int x, const int y)
        {
            if (m_is_selecting)
                finnish_select();

            scrollable_panel<widget_type>::on_mouse_drag_end(button, x, y);
            return true;
        }

        template<class widget_type>
        void edit_panel<widget_type>::start_select(const int x, const int y)
        {
            m_selection_start_x = x;
            m_selection_start_y = y;
            m_selection_end_x = x;
            m_selection_end_y = y;
            scrollable_panel<widget_type>::set_scrollable(false);
            m_is_selecting = true;
        }

        template<class widget_type>
        void edit_panel<widget_type>::finnish_select()
        {
            m_is_selecting = false;
            scrollable_panel<widget_type>::set_scrollable(true);
            scrollable_panel<widget_type>::redraw();
        }

        template<class widget_type>
        void edit_panel<widget_type>::update_selection(const int x, const int y)
        {
            m_selection_end_x = x;
            m_selection_end_y = y;

            m_selected_area.x = std::min(m_selection_start_x, m_selection_end_x);
            m_selected_area.y = std::min(m_selection_start_y, m_selection_end_y);
            m_selected_area.width = std::abs(m_selection_start_x - m_selection_end_x);
            m_selected_area.height = std::abs(m_selection_start_y - m_selection_end_y);

            rectangle child_area
            {
                scrollable_panel<widget_type>::convert_x(m_selected_area.x),
                scrollable_panel<widget_type>::convert_y(m_selected_area.y),
                m_selected_area.width,
                m_selected_area.height
            };

            m_selection.clear();

            for (auto& w : scrollable_panel<widget_type>::m_widgets) {
                if (child_area.overlap(w->get_absolute_rect())) {
                    m_selection.push_back(w.get());

                    if (!w->is_selected()) {
                        w->m_is_selected = true;
                        w->on_select();
                    }
                }
                else if (w->is_selected()) {
                    w->m_is_selected = false;
                    w->on_unselect();
                }
            }

            scrollable_panel<widget_type>::redraw();
        }

        template<class widget_type>
        void edit_panel<widget_type>::unselect_all()
        {
            for (auto w : m_selection) {
                if (w->is_selected()) {
                    w->m_is_selected = false;
                    w->on_unselect();
                }
            }
            m_selection.clear();
            scrollable_panel<widget_type>::redraw();
        }

    } /* View */

} /* Gammou */

#endif
