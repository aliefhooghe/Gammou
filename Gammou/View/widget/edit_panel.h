#ifndef EDIT_PANEL_H_
#define EDIT_PANEL_H_

#include <vector>
#include "scrollable_panel.h"

namespace Gammou {

    namespace View {

        class edit_widget : public panel<> {

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
                edit_panel(const int x, const int y,
                           const unsigned int width,
                           const unsigned int height,
                           const color background = cl_white);
                edit_panel(const rectangle& rect, const color background = cl_white);
                virtual ~edit_panel();

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

                virtual void remove_widget(widget_type *w) override;
                const std::vector<widget_type*> get_selection() const;

             private:
               enum class state
               {
                   SCROLL,
                   SELECTING,
                   MOVING
               };
               void set_state(const state s);

                //  Selecting
                void start_select(const int x, const int y);
                void finnish_select();
                void update_selection(const int x, const int y);

                void select(widget_type *w);
                void unselect_all();

                int m_selection_start_x;
                int m_selection_start_y;
                rectangle m_selected_area;
                std::vector<widget_type*> m_selection{};

                state m_state;
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
            set_state(state::SCROLL);
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

            if (m_state == state::SELECTING) {
                double dash_width = 1.0;
                cairo_set_dash (cr, &dash_width, 1, 0);

                cairo_set_line_width(cr, 1.0);
                cairo_helper::set_source_color(cr, cl_gray);
                cairo_helper::simple_rectangle(cr, m_selected_area);
                cairo_stroke(cr);
            }
        }

        template<class widget_type>
        void edit_panel<widget_type>::remove_widget(widget_type *w)
        {
            if (w->is_selected())
                m_selection.erase(
                    std::remove(
                        m_selection.begin(), m_selection.end(), w),
                        m_selection.end());
            scrollable_panel<widget_type>::remove_widget(w);
        }

        template<class widget_type>
        const std::vector<widget_type*> edit_panel<widget_type>::get_selection() const
        {
            return m_selection;
        }

        template<class widget_type>
        bool edit_panel<widget_type>::on_mouse_button_up(
                const mouse_button button,
                const int x, const int y)
        {
            if (m_state == state::SELECTING)
                finnish_select();
            else if (m_state == state::MOVING)
                set_state(state::SCROLL);
            else if (scrollable_panel<widget_type>::get_focused_widget() == nullptr)
                unselect_all();

            scrollable_panel<widget_type>::on_mouse_button_up(button, x, y);
            return true;
        }

        template<class widget_type>
        bool edit_panel<widget_type>::on_mouse_drag_start(
            const mouse_button button, const int x, const int y)
        {
            if (widget::get_key_state(key_ctrl_left)) {
                start_select(x, y);
            }
            else if (auto w = scrollable_panel<widget_type>::get_focused_widget()) {
                if (!w->is_selected()) {
                    unselect_all();
                    select(w);
                }
                set_state(state::MOVING);
            }

            scrollable_panel<widget_type>::on_mouse_drag_start(button, x, y);
            return true;
        }

        template<class widget_type>
        bool edit_panel<widget_type>::on_mouse_drag(
            const mouse_button button,
            const int x, const int y,
            const int dx, const int dy)
        {
            const bool ret =
                scrollable_panel<widget_type>::on_mouse_drag(button, x, y, dx, dy);

            //  event have not been used
            if (m_state == state::MOVING && !ret) {
                //  Move selection
                for (auto& select_w : m_selection)
                    select_w->set_rect(select_w->get_absolute_rect().translate(dx, dy));
            }
            else if (m_state == state::SELECTING) {
                update_selection(x, y);
            }

            return true;
        }

        template<class widget_type>
        bool edit_panel<widget_type>::on_mouse_drag_end(
            const mouse_button button,
            const int x, const int y)
        {
            if (m_state == state::SELECTING)
                finnish_select();
            else if (m_state == state::MOVING)
                set_state(state::SCROLL);

            scrollable_panel<widget_type>::on_mouse_drag_end(button, x, y);
            return true;
        }

        template<class widget_type>
        void edit_panel<widget_type>::set_state(const state s)
        {
            m_state = s;
            scrollable_panel<widget_type>::set_scrollable(s == state::SCROLL);
        }

        template<class widget_type>
        void edit_panel<widget_type>::start_select(const int x, const int y)
        {
            m_selection_start_x = x;
            m_selection_start_y = y;
            set_state(state::SELECTING);
        }

        template<class widget_type>
        void edit_panel<widget_type>::finnish_select()
        {
            set_state(state::SCROLL);
            scrollable_panel<widget_type>::redraw();
        }

        template<class widget_type>
        void edit_panel<widget_type>::update_selection(const int x, const int y)
        {
            m_selected_area.x = std::min(m_selection_start_x, x);
            m_selected_area.y = std::min(m_selection_start_y, y);
            m_selected_area.width = std::abs(m_selection_start_x - x);
            m_selected_area.height = std::abs(m_selection_start_y - y);

            rectangle child_area
            {
                scrollable_panel<widget_type>::convert_x(m_selected_area.x),
                scrollable_panel<widget_type>::convert_y(m_selected_area.y),
                m_selected_area.width,
                m_selected_area.height
            };

            //  Really suboptimal algo, TODO mieux
            unselect_all();

            for (auto& w : scrollable_panel<widget_type>::m_widgets)
                if (child_area.overlap(w->get_absolute_rect()))
                    select(w.get());

            scrollable_panel<widget_type>::redraw();
        }

        template<class widget_type>
        void edit_panel<widget_type>::select(widget_type *w)
        {
            if (!w->is_selected()) {
                w->m_is_selected = true;
                w->on_select();
                m_selection.push_back(w);
            }
        }

        template<class widget_type>
        void edit_panel<widget_type>::unselect_all()
        {
            for (auto w : m_selection) {
                w->m_is_selected = false;
                w->on_unselect();
            }
            m_selection.clear();
            scrollable_panel<widget_type>::redraw();
        }

    } /* View */

} /* Gammou */

#endif
