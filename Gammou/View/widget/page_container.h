#ifndef GAMMOU_PAGE_CONTAINER_H_
#define GAMMOU_PAGE_CONTAINER_H_

#include <vector>
#include "panel.h"

namespace Gammou {

	namespace View {

		class page_container : public abstract_panel {

		public:
			page_container(const int x, const int y, const unsigned int width, 
				const unsigned int height, const color background);
			page_container(const rectangle& rect, const color background);

			virtual ~page_container();

			virtual bool on_key_up(const keycode key) override;
			virtual bool on_key_down(const keycode key) override;

			virtual bool on_mouse_enter(void) override;
			virtual bool on_mouse_exit(void) override;
			virtual bool on_mouse_move(const int x, const int y) override;
			virtual bool on_mouse_wheel(const float distance) override;

			virtual bool on_mouse_button_down(const mouse_button button, const int x, const int y) override;
			virtual bool on_mouse_button_up(const mouse_button button, const int x, const int y) override;
			virtual bool on_mouse_dbl_click(const int x, const int y) override;

			virtual bool on_mouse_drag(const mouse_button button, const int x, const int y,
				const int dx, const int dy) override;
			virtual bool on_mouse_drag_start(const mouse_button button, const int x, const int y) override;
			virtual bool on_mouse_drag_end(const mouse_button button, const int x, const int y) override;

			void add_page(widget *page = nullptr);
			void reset_page(const unsigned int page_id, widget *page = nullptr);
			unsigned int get_page_count() const;
			widget *get_page(const unsigned int page_id) const;
			void select_page(const unsigned int page_id);

			virtual void draw(cairo_t *cr) override;
		private:
			widget *get_current_page() const;
			bool check_widget_size(const widget *w) const;

			std::vector<widget*> m_pages;
			int m_current_page_id;
		};

	} /* View */

} /* Gammou */



#endif