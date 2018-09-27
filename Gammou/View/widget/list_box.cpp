
#include <algorithm>
#include "list_box.h"

namespace Gammou {

	namespace View {

		const float list_box::epsilon = 2.0f;

		list_box::list_box(
			const int x, const int y, const unsigned int width, const unsigned int height, 
			const unsigned int displayed_items_count, 
			const color selected_item_color, 
			const color hovered_item_color,
			const color background, 
			const color font_color, 
			const unsigned int font_size)
			: widget(x, y, width, height),
			m_selected_color(selected_item_color),
			m_hovered_color(hovered_item_color),
			m_background_color(background),
			m_font_color(font_color),
			m_font_size(font_size),
			m_displayed_item_count(displayed_items_count),
			m_item_height(
				(static_cast<float>(get_height()) - 2.0f * epsilon) / 
				static_cast<float>(displayed_items_count)),
			m_item_width(
				static_cast<float>(get_width() - 2.0f * epsilon)),
			m_selected_id(-1),
			m_hovered_id(-1),
			m_first_displayed(0)
		{
		}

		list_box::list_box(
			const rectangle & rect,
			const unsigned int displayed_items_count, 
			const color selected_item_color, 
			const color hovered_item_color,
			const color background, 
			const color font_color, 
			const unsigned int font_size)
			: widget(rect),
			m_selected_color(selected_item_color),
			m_hovered_color(hovered_item_color),
			m_background_color(background),
			m_font_color(font_color),
			m_font_size(font_size),
			m_displayed_item_count(displayed_items_count),
			m_item_height(
				(static_cast<float>(get_height()) - 2.0f * epsilon) / 
				static_cast<float>(displayed_items_count)),
			m_item_width(
				static_cast<float>(get_width() - 2.0f * epsilon)),
			m_selected_id(-1),
			m_hovered_id(-1),
			m_first_displayed(0)
		{
        }

        list_box::~list_box()
        {
        }

        void list_box::set_item_select_event(std::function<void(list_box&, unsigned int)> handler)
        {
            m_item_select_handler = handler;
        }

        void list_box::set_item_dbl_clik_event(std::function<void(list_box&, unsigned int)> handler)
        {
            m_item_dbl_click_handler = handler;
        }

		unsigned int list_box::get_item_count() const
		{
			return static_cast<unsigned int>(m_items.size());
		}

		unsigned int list_box::add_item(const std::string & name)
		{
			m_items.push_back(name);
			redraw();
			return static_cast<unsigned int>(m_items.size() - 1);
		}

		void list_box::clear()
		{
			if (get_item_count() > 0) {
				m_items.clear();
				m_selected_id = -1;
				m_first_displayed = 0;
				redraw();
			};
		}

		int list_box::get_selected_item() const
		{
			return m_selected_id;
		}

		void list_box::select_item(const unsigned int id)
		{
			if (id < m_items.size())
				m_selected_id = static_cast<int>(id);
		}

		bool list_box::on_mouse_button_up(const mouse_button button, const int x, const int y)
		{
			if (button == mouse_button::LeftButton) {
				update_selected(y);
				return true;
			}

			return false;
		}

		bool list_box::on_mouse_drag_end(const mouse_button button, const int x, const int y)
		{
            if (button == mouse_button::LeftButton) {
                update_selected(y);
                return true;
            }

            return false;
		}

		bool list_box::on_key_down(const keycode key)
		{
			if (key == key_up_arrow) {
				scroll(1);
				return true;
			}
			else if (key == key_down_arrow) {
				scroll(-1);
				return true;
			}
			else {
				return false;
			}
		}

		bool list_box::on_mouse_exit(void)
		{
			if (m_hovered_id != -1) {
				m_hovered_id = -1;
				redraw();
			}
			return true;
		}

		bool list_box::on_mouse_move(const int x, const int y)
		{
			const int id = id_by_pos(y);

			if (id != -1 && id != m_hovered_id) {
				m_hovered_id = id;
				redraw();
			}

			return true;
		}

		bool list_box::on_mouse_wheel(const float distance)
		{
			scroll(static_cast<int>(distance));
			return true;
		}

		void list_box::draw(cairo_t * cr)
		{
			const unsigned int end_displayed =
				std::min<int>(
					static_cast<int>(m_items.size()), 
					m_first_displayed + m_displayed_item_count);

			// Background
            cairo_rectangle(cr, 0, 0, static_cast<double>(get_width()), static_cast<double>(get_height()));
			cairo_helper::set_source_color(cr, m_background_color);
			cairo_fill(cr);

			// Items
			cairo_set_font_size(cr, m_font_size);
			cairo_select_font_face(cr, "sans-serif", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
			cairo_helper::set_source_color(cr, m_font_color);

			unsigned int j = 0;
			for (unsigned int i = m_first_displayed; i < end_displayed; ++i, ++j) {
				const rectangle rect(
                    static_cast<int>(epsilon), // for text
					static_cast<int>(epsilon + static_cast<float>(j) * m_item_height),
					static_cast<unsigned int>(m_item_width),
					static_cast<unsigned int>(m_item_height));

				if (static_cast<int>(i) == m_selected_id) {
					cairo_helper::set_source_color(cr, m_selected_color);
					cairo_helper::simple_rectangle(cr, rect);
					cairo_fill(cr);
					cairo_helper::set_source_color(cr, m_font_color);
				}
				else if (static_cast<int>(i) == m_hovered_id) {
					cairo_helper::set_source_color(cr, m_hovered_color);
					cairo_helper::simple_rectangle(cr, rect);
					cairo_fill(cr);
					cairo_helper::set_source_color(cr, m_font_color);
				}

				cairo_helper::show_left_aligned_text(cr, rect, m_items[i], epsilon);
			}
		}

		void list_box::scroll(const int distance)
		{
			if (m_items.size() != 0) {
				const int n = static_cast<int>(m_first_displayed) - distance;
				const int limit
					= std::max<int>(0, static_cast<int>(m_items.size()) - static_cast<int>(m_displayed_item_count));

				if (n < 0)
					m_first_displayed = 0;
				else if (n >= limit)
					m_first_displayed = limit;
				else
					m_first_displayed = n;

				redraw();
			}
		}

		int list_box::id_by_pos(const int y)
		{
			const int offset =
				static_cast<int>(static_cast<float>(y - epsilon) / 
				m_item_height);

			if (offset >= 0 && 
				static_cast<unsigned int>(offset) < m_displayed_item_count) {
				const int id = m_first_displayed + offset;

				if (id < static_cast<int>(m_items.size()))
					return id;
			}
			
			return -1;
		}

		void list_box::update_selected(const int y)
		{
			const int id = id_by_pos(y);

			if (id != -1 && id != m_selected_id) {
				m_selected_id = id;
				if (m_item_select_handler) {
                    m_item_select_handler(
                        *this, static_cast<unsigned int>(id));
					redraw();
				}
			}
		}

	} /* View */

} /* Gammou */
