
#include <algorithm>
#include "list_box.h"

namespace Gammou {

	namespace View {



		list_box::list_box(
			const int x, const int y, const unsigned int width, const unsigned int height, 
			const unsigned int displayed_items_count, 
			std::function<void(unsigned int)> on_select,
			const color selected_item_color, 
			const color background, 
			const color border_color, 
			const color font_color, 
			const unsigned int font_size,
			const float border_width)
			: widget(x, y, width, height),
			m_on_select(on_select),
			m_selected_color(selected_item_color),
			m_background_color(background),
			m_font_color(font_color),
			m_border_color(border_color),
			m_font_size(font_size),
			m_border_width(border_width),
			m_item_height(static_cast<float>(get_height()) / static_cast<float>(displayed_items_count)),
			m_displayed_item_count(displayed_items_count),
			m_selected_id(-1),
			m_first_displayed(0)
		{
		}

		list_box::list_box(
			const rectangle & rect,
			const unsigned int displayed_items_count, 
			std::function<void(unsigned int)> on_select,
			const color selected_item_color, 
			const color background, 
			const color border_color, 
			const color font_color, 
			const unsigned int font_size,
			const float border_width)
			: widget(rect),
			m_on_select(on_select),
			m_selected_color(selected_item_color),
			m_background_color(background),
			m_font_color(font_color),
			m_border_color(border_color),
			m_font_size(font_size),
			m_border_width(border_width),
			m_item_height(
				(static_cast<float>(get_height()) - 2.0f * border_width)/
				static_cast<float>(displayed_items_count)),
			m_displayed_item_count(displayed_items_count),
			m_selected_id(-1),
			m_first_displayed(0)
		{
		}

		unsigned int list_box::get_item_count() const
		{
			return static_cast<unsigned int>(m_items.size());
		}

		unsigned int list_box::add_item(const std::string & name)
		{
			m_items.push_back(name);
			return static_cast<unsigned int>(m_items.size() - 1);
		}

		void list_box::clear()
		{
			if (get_item_count() > 0) {
				m_items.clear();
				m_selected_id = -1;
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
			cairo_rectangle(cr, 0, 0, static_cast<float>(get_width()), static_cast<float>(get_height()));
			cairo_helper::set_source_color(cr, m_background_color);
			cairo_fill(cr);


			// Items
			cairo_set_font_size(cr, m_font_size);
			cairo_select_font_face(cr, "sans-serif", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
			cairo_helper::set_source_color(cr, m_font_color);

			unsigned int j = 0;
			for (unsigned int i = m_first_displayed; i < end_displayed; ++i, ++j) {
				const rectangle rect(
					5, // for text
					static_cast<int>(m_border_width + static_cast<float>(j) * m_item_height),
					get_width(),
					static_cast<unsigned int>(m_item_height));

				if (static_cast<int>(i) == get_selected_item()) {
					cairo_helper::set_source_color(cr, m_selected_color);
					cairo_rectangle(cr, 0, rect.y, rect.width, rect.height);
					cairo_fill(cr);
					cairo_helper::set_source_color(cr, m_font_color);
				}

				//cairo_helper::show_centered_text(cr, rect, m_items[i]);
				cairo_helper::show_left_aligned_text(cr, rect, m_items[i]);
			}

			// Border (after Items to avoid overlap)
			cairo_rectangle(cr, 
				0.5f * m_border_width, 0.5f * m_border_width, 
				static_cast<float>(get_width()) - m_border_width, 
				static_cast<float>(get_height()) - m_border_width);
			cairo_helper::set_source_color(cr, m_border_color);
			cairo_stroke(cr);
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

		void list_box::update_selected(const int y)
		{
			const int offset =
				static_cast<int>(static_cast<float>(y) / m_item_height);

			if (offset >= 0 && 
				static_cast<unsigned int>(offset) < m_displayed_item_count) {

				const int new_selected = m_first_displayed + offset;

				if (new_selected < static_cast<int>(m_items.size()) &&
					new_selected != m_selected_id) {
					m_selected_id = new_selected;
					m_on_select(m_selected_id);
					redraw();
				}
			}
		}



	} /* View */

} /* Gammou */