
#include <vector>
#include <string>
#include "panel.h"

namespace Gammou {

	namespace View {



		class list_box : public widget {

		public: 
			list_box(const unsigned int x, const unsigned int y, const unsigned int width, const unsigned int height, 
				const unsigned int displayed_items_count, const color selected_item_color, const color background, 
				const color font_color, const unsigned int font_size);
			list_box(const rectangle& rect, const unsigned int displayed_items_count,const color selected_item_color, const color background,
				const color font_color, const unsigned int font_size);
			~list_box() {}


			// No remove function because it would cause issues with id
			unsigned int add_item(const std::string& name); // Todo item description ?
			int get_selected_item() const;					//	-1 if nothing selected
			void select_item(const unsigned int id);

			bool on_mouse_button_up(const mouse_button button, const int x, const int y);
			bool on_mouse_drag_end(const mouse_button button, const int x, const int y);
			bool on_mouse_wheel(const float distance);
			void draw(cairo_t *cr);

		private:
			void update_selected(const int y);

			const color m_selected_color;
			const color m_background_color;
			const color m_font_color;
			const unsigned int m_font_size;
			const float m_item_height;
			const unsigned int m_displayed_item_count;

			std::vector<std::string> m_items;
			int m_selected_id;
			unsigned int m_first_displayed;
		};

	}

}