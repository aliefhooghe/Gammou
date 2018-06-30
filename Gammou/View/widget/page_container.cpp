
#include "page_container.h"

namespace Gammou {

	namespace View {

		page_container::page_container(
			const int x, const int y, 
			const unsigned int width, const unsigned int height, 
			const color background)
			:	abstract_panel(x, y, width, height, background) 

		{
		}

		page_container::page_container(const rectangle & rect, const color background)
			:	abstract_panel(rect, background)
		{
		}

		page_container::~page_container()
		{
		}

		bool page_container::on_key_up(const keycode key)
		{
			widget *w = get_current_page();

			if (w == nullptr)
				return false;
			else
				return w->on_key_up(key);
		}

		bool page_container::on_key_down(const keycode key)
		{
			widget *w = get_current_page();

			if (w == nullptr)
				return false;
			else
				return w->on_key_down(key);
		}

		bool page_container::on_mouse_enter(void)
		{
			widget *w = get_current_page();

			if (w == nullptr)
				return false;
			else
				return w->on_mouse_enter();
		}

		bool page_container::on_mouse_exit(void)
		{
			widget *w = get_current_page();

			if (w == nullptr)
				return false;
			else
				return w->on_mouse_exit();
		}

		bool page_container::on_mouse_move(const int x, const int y)
		{
			widget *w = get_current_page();

			if (w == nullptr)
				return false;
			else
				return w->on_mouse_move(x, y);
		}

		bool page_container::on_mouse_wheel(const float distance)
		{
			widget *w = get_current_page();

			if (w == nullptr)
				return false;
			else
				return w->on_mouse_wheel(distance);
		}

		bool page_container::on_mouse_button_down(const mouse_button button, const int x, const int y)
		{
			widget *w = get_current_page();

			if (w == nullptr)
				return false;
			else
				return w->on_mouse_button_down(button, x, y);
		}

		bool page_container::on_mouse_button_up(const mouse_button button, const int x, const int y)
		{
			widget *w = get_current_page();

			if (w == nullptr)
				return false;
			else
				return w->on_mouse_button_up(button, x, y);
		}

		bool page_container::on_mouse_dbl_click(const int x, const int y)
		{
			widget *w = get_current_page();

			if (w == nullptr)
				return false;
			else
				return w->on_mouse_dbl_click(x, y);
		}

		bool page_container::on_mouse_drag(const mouse_button button, const int x, const int y, const int dx, const int dy)
		{
			widget *w = get_current_page();

			if (w == nullptr)
				return false;
			else
				return w->on_mouse_drag(button, x, y, dx, dy);
		}

		bool page_container::on_mouse_drag_start(const mouse_button button, const int x, const int y)
		{
			widget *w = get_current_page();

			if (w == nullptr)
				return false;
			else
				return w->on_mouse_drag_start(button, x, y);
		}

		bool page_container::on_mouse_drag_end(const mouse_button button, const int x, const int y)
		{
			widget *w = get_current_page();

			if (w == nullptr)
				return false;
			else
				return w->on_mouse_drag_end(button, x, y);
		}

		void page_container::add_page(std::unique_ptr<widget> && page)
		{
			if ( !check_widget_size(page.get()))
				throw std::domain_error("Widget must have exactly the page size and be at (0,0) ");

			get_ownership(*page);
			m_pages.push_back(std::move(page));
		}

		void page_container::reset_page(const unsigned int page_id, std::unique_ptr<widget> && page)
		{
			if (page_id >= get_page_count())
				throw std::domain_error("Invalid page id");

			if(!check_widget_size(page.get()))
				throw std::domain_error("Widget must have exactly the page size and be at (0,0) ");

			release_widget(*(m_pages[page_id]));
			get_ownership(*page);

			m_pages[page_id] = std::move(page);

			if (page_id == m_current_page_id)
				redraw();
		}

		unsigned int page_container::get_page_count() const
		{
			return static_cast<unsigned int>(m_pages.size());
		}

		widget& page_container::get_page(const unsigned int page_id) const
		{
			if (page_id >= get_page_count())
				throw std::domain_error("Invalid page id");
			return *(m_pages[page_id]);
		}

		void page_container::select_page(const unsigned int page_id)
		{
			if (page_id >= get_page_count())
				throw std::domain_error("Invalid page id");

			if (m_current_page_id != page_id) {
				m_current_page_id = page_id;
				redraw();
			}
		}

		void page_container::draw(cairo_t * cr)
		{
			widget *w = get_current_page();

			if (w == nullptr)
				draw_background(cr);
			else
				w->draw(cr);
		}

		widget *page_container::get_current_page() const
		{
			if (m_current_page_id == -1) // no page selected
				return nullptr;
			else
				return m_pages[m_current_page_id].get();
		}

		bool page_container::check_widget_size(const widget * w) const
		{
			return (
				w->get_x() == 0 ||
				w->get_y() == 0 ||
				w->get_width() == get_width() ||
				w->get_height() == get_height());
		}



	} /* View */

} /* Gammou */