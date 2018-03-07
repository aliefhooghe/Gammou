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
				std::function<void(unsigned int)> on_select = [](unsigned int id) {},
				const color selected_item_color = cl_azure, 
				const color background = cl_white, 
				const color border_color = cl_black, 
				const color font_color = cl_black, 
				const unsigned int font_size = 11);

			list_box(
				const rectangle& rect, 
				const unsigned int displayed_items_count,
				std::function<void(unsigned int)> on_select = [](unsigned int id) {},
				const color selected_item_color = cl_azure,
				const color background = cl_white,
				const color border_color = cl_black,
				const color font_color = cl_black,
				const unsigned int font_size = 11);

			~list_box() {}


			// No remove function because it would cause issues with id
			unsigned int add_item(const std::string& name); // Todo item description ?
			int get_selected_item() const;					//	-1 if nothing selected
			void select_item(const unsigned int id);

			bool on_mouse_button_up(const mouse_button button, const int x, const int y) override;
			bool on_mouse_drag_end(const mouse_button button, const int x, const int y) override;

			bool on_key_down(const keycode key) override;
			bool on_mouse_wheel(const float distance) override;
			
			void draw(cairo_t *cr) override;

		private:
			void scroll(const int distance);
			void update_selected(const int y);

			std::function<void(unsigned int)> m_on_select;

			const color m_selected_color;
			const color m_background_color;
			const color m_font_color;
			const color m_border_color;
			const unsigned int m_font_size;
			const float m_item_height;
			const unsigned int m_displayed_item_count;

			std::vector<std::string> m_items;
			int m_selected_id;
			unsigned int m_first_displayed;
		};

	}

}

#endif