#ifndef GAMMOU_LIST_BOX_H
#define GAMMOU_LIST_BOX_H

#include <functional>

#include <vector>
#include <string>
#include "panel.h"

namespace Gammou {

	namespace View {



		class list_box : public widget {

		public: 
			list_box(
				const int x, 
				const int y, 
				const unsigned int width, 
				const unsigned int height, 
				const unsigned int displayed_items_count, 
				const color selected_item_color = cl_lightgrey, 
				const color hovered_item_color = cl_lemonchiffon,
				const color background = cl_white, 
				const color font_color = cl_black, 
				const unsigned int font_size = 11);

			list_box(
				const rectangle& rect, 
				const unsigned int displayed_items_count,
				const color selected_item_color = cl_lightgrey,
				const color hovered_item_color = cl_lemonchiffon,
				const color background = cl_white,
				const color font_color = cl_black,
				const unsigned int font_size = 11);

            ~list_box() override;

            void set_item_select_event(std::function<void(list_box&, unsigned int)> handler);
            void set_item_dbl_clik_event(std::function<void(list_box&, unsigned int)> handler);

			// No remove function because it would cause issues with id
			unsigned int get_item_count() const;
			unsigned int add_item(const std::string& name); // Todo item description ?
			void clear();
			int get_selected_item() const;					//	-1 if nothing selected
			void select_item(const unsigned int id);

			bool on_mouse_button_up(const mouse_button button, const int x, const int y) override;
			bool on_mouse_drag_end(const mouse_button button, const int x, const int y) override;

			bool on_key_down(const keycode key) override;

			bool on_mouse_exit(void) override;
			bool on_mouse_move(const int x, const int y) override;
			bool on_mouse_wheel(const float distance) override;
			
			void draw(cairo_t *cr) override;

		private:
			void scroll(const int distance);
			int id_by_pos(const int y);
			void update_selected(const int y);

            std::function<void(list_box&, unsigned int)> m_item_select_handler{};
            std::function<void(list_box&, unsigned int)> m_item_dbl_click_handler{};

			const color m_selected_color;
			const color m_hovered_color;
			const color m_background_color;
			const color m_font_color;
			const unsigned int m_font_size;
			const unsigned int m_displayed_item_count;

			static const float epsilon;
			const float m_item_width;
			const float m_item_height;

			std::vector<std::string> m_items;
			int m_selected_id;
			int m_hovered_id;
			unsigned int m_first_displayed;
		};

	}

}

#endif
