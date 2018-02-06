#ifndef GAMMOU_SCROLLABLE_PANEL_H_
#define GAMMOU_SCROLLABLE_PANEL_H_

#include "panel.h"

namespace Gammou {

	namespace View {

		template<class widget_type = widget>
		class scrollable_panel : public panel<widget_type> {

		public:
			enum class scroll_method { WHEEL_SCROLL, DRAG_SCROLL };

			scrollable_panel(const unsigned int x, const unsigned int y, const unsigned int width,
				const unsigned int height, const color background = cl_white);
			scrollable_panel(const rectangle& rect, const color background = cl_white);

			virtual ~scrollable_panel();

			virtual bool on_mouse_move(const int x, const int y) override;
			virtual bool on_mouse_button_down(const mouse_button button, const int x, const int y) override;
			virtual bool on_mouse_button_up(const mouse_button button, const int x, const int y) override;
			virtual bool on_mouse_dbl_click(const int x, const int y) override;
			virtual bool on_mouse_wheel(const float distance) override;

			virtual bool on_mouse_drag_start(const mouse_button button, const int x, const int y) override;
			virtual bool on_mouse_drag(const mouse_button button, const int x, const int y,
				const int dx, const int dy) override;
			virtual bool on_mouse_drag_end(const mouse_button button, const int x, const int y) override;

			// Cannot be overrided, override draw_content instead
			void draw(cairo_t *cr) override;

			bool contains(const int px, const int py) const override;
			bool contains(const rectangle & rect) const override;
			void redraw_rect(const rectangle& rect) override;

			using panel<widget_type>::add_widget; // For test only
		protected:
			virtual void draw_content(cairo_t *cr);

			void scroll(const int dx, const int dy);
			void set_scrollable(const bool scrolable = true);
			void set_sroll_method(const scroll_method method);

			unsigned int convert_x(const unsigned int x);
			unsigned int convert_y(const unsigned int y);
		private:
			unsigned int m_x_origin;
			unsigned int m_y_origin;
			bool m_scrolable;
			scroll_method m_scroll_method;
		};

		template<class widget_type>
		scrollable_panel<widget_type>::scrollable_panel(const unsigned int x, const unsigned int y, const unsigned int width, 
			const unsigned int height, const color background)
			: panel<widget_type>(x, y, width, height, background),
			m_x_origin(0),
			m_y_origin(0),
			m_scrolable(true),
			m_scroll_method(scroll_method::WHEEL_SCROLL)
		{
		}

		template<class widget_type>
		scrollable_panel<widget_type>::scrollable_panel(const rectangle & rect, const color background)
			: panel<widget_type>(rect, background),
			m_x_origin(0),
			m_y_origin(0),
			m_scrolable(true),
			m_scroll_method(scroll_method::WHEEL_SCROLL)
		{
		}

		template<class widget_type>
		scrollable_panel<widget_type>::~scrollable_panel()
		{
		}

		template<class widget_type>
		bool scrollable_panel<widget_type>::on_mouse_move(const int x, const int y)
		{
			return panel<widget_type>::on_mouse_move(convert_x(x), convert_y(y));
		}

		template<class widget_type>
		bool scrollable_panel<widget_type>::on_mouse_button_down(const mouse_button button, const int x, const int y)
		{
			return panel<widget_type>::on_mouse_button_down(button, convert_x(x), convert_y(y));
		}

		template<class widget_type>
		bool scrollable_panel<widget_type>::on_mouse_button_up(const mouse_button button, const int x, const int y)
		{
			 return panel<widget_type>::on_mouse_button_up(button, convert_x(x), convert_y(y));
		}

		template<class widget_type>
		bool scrollable_panel<widget_type>::on_mouse_dbl_click(const int x, const int y)
		{
			return panel<widget_type>::on_mouse_dbl_click(convert_x(x), convert_y(y));
		}

		template<class widget_type>
		bool scrollable_panel<widget_type>::on_mouse_wheel(const float distance)
		{
			const bool ret = panel<widget_type>::on_mouse_wheel(distance);

			if (m_scrolable && !ret &&
				m_scroll_method == scroll_method::WHEEL_SCROLL) {
				scroll(0, -20 * static_cast<int>(distance));
			}

			return true;
		}

		template<class widget_type>
		bool scrollable_panel<widget_type>::on_mouse_drag_start(const mouse_button button, const int x, const int y)
		{
			return panel<widget_type>::on_mouse_drag_start(button, convert_x(x), convert_y(y));
		}

		template<class widget_type>
		 bool scrollable_panel<widget_type>::on_mouse_drag(const mouse_button button, const int x, const int y, const int dx, const int dy)
		{
			bool ret = panel<widget_type>::on_mouse_drag(button, convert_x(x), convert_y(y), dx, dy);

			if (m_scrolable && !ret &&
				m_scroll_method == scroll_method::DRAG_SCROLL) {
				scroll(-dx, -dy);
			}

			return true;
		}

		template<class widget_type>
		bool scrollable_panel<widget_type>::on_mouse_drag_end(const mouse_button button, const int x, const int y)
		{
			return panel<widget_type>::on_mouse_drag_end(button, convert_x(x), convert_y(y));
		}

		template<class widget_type>
		void scrollable_panel<widget_type>::draw(cairo_t * cr)
		{
			draw_background(cr); // Do cliping
			cairo_save(cr);
			cairo_translate(cr, -static_cast<int>(m_x_origin), -static_cast<int>(m_y_origin));
			draw_content(cr);
			cairo_restore(cr);
		}

		template<class widget_type>
		bool scrollable_panel<widget_type>::contains(const int px, const int py) const
		{
			return (px >= 0 && py >= 0);
		}

		template<class widget_type>
		bool scrollable_panel<widget_type>::contains(const rectangle & rect) const
		{
			return (rect.x >= 0 && rect.y >= 0);
		}

		template<class widget_type>
		void scrollable_panel<widget_type>::redraw_rect(const rectangle & rect)
		{
			abstract_panel *const parent = get_parent();

			if (parent != nullptr) {
				rectangle real_rect = rect.translate(-static_cast<int>(m_x_origin), -static_cast<int>(m_y_origin));

				// if we contain at least one point of the rect
				if (real_rect.x <= static_cast<int>(get_width()) &&
					real_rect.y <= static_cast<int>(get_height()) &&
					0 <= real_rect.x + static_cast<int>(real_rect.width) &&
					0 <= real_rect.y + static_cast<int>(real_rect.height)) {
					parent->redraw_rect(real_rect.translate(get_x() , get_y()));
				}
			}
		}

		template<class widget_type>
		void scrollable_panel<widget_type>::draw_content(cairo_t * cr)
		{
			// Default implementation
			panel<widget_type>::draw_widgets(cr);
		}

		template<class widget_type>
		 void scrollable_panel<widget_type>::scroll(const int dx, const int dy)
		 {
			 const int new_x_origin = dx + static_cast<int>(m_x_origin);
			 const int new_y_origin = dy + static_cast<int>(m_y_origin);

			 if (new_y_origin > 0)
				 m_y_origin = static_cast<unsigned int>(new_y_origin);
			 else
				 m_y_origin = 0;

			 if (new_x_origin > 0)
				 m_x_origin = static_cast<unsigned int>(new_x_origin);
			 else
				 m_x_origin = 0;

			 DEBUG_PRINT("SCROLL %d %d\n", dx, dy);
			 redraw();
		 }
		 
		 template<class widget_type>
		 void scrollable_panel<widget_type>::set_scrollable(const bool scrolable)
		 {
			 m_scrolable = scrolable;
		 }

		 template<class widget_type>
		 void scrollable_panel<widget_type>::set_sroll_method(const scroll_method method)
		 {
			 m_scroll_method = method;
		 }

		 template<class widget_type>
		 unsigned int scrollable_panel<widget_type>::convert_x(const unsigned int x)
		 {
			 return x + m_x_origin;
		 }

		 template<class widget_type>
		 unsigned int scrollable_panel<widget_type>::convert_y(const unsigned int y)
		 {
			 return y + m_y_origin;
		 }



	} /* View */

} /* Gammou */


#endif