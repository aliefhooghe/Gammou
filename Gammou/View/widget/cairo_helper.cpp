#define  _USE_MATH_DEFINES
#include <cmath>
#include <algorithm>
#include "cairo_helper.h"

namespace Gammou {

	namespace View {

		rectangle::rectangle()
			: x(0), y(0), width(0), height(0)
		{
		}

        rectangle::rectangle(const int px, const int py, const unsigned int width, const unsigned int height)
			: x(px), y(py), width(width), height(height)
		{
		}

		rectangle rectangle::translate(const int px, const int py) const
		{
			return rectangle(x + px, y + py, width, height);
		}

		bool rectangle::contains(const int px, const int py) const
		{
            return (px >= x && px < (x + static_cast<int>(width)))
                && (py >= y && py < (y + static_cast<int>(height)));
		}

		bool rectangle::contains(const rectangle & rect) const
		{
            const auto right = rect.x + static_cast<int>(rect.width);
            const auto bottom = rect.y + static_cast<int>(rect.height);

			return contains(rect.x, rect.y)
                && contains(rect.x, bottom)
                && contains(right, rect.y)
                && contains(right, bottom);
		}

        bool rectangle::overlap(const rectangle &rect) const
        {
            const int left = std::max(x, rect.x);
            const int right = std::min(
                    x + static_cast<int>(width),
                    rect.x + static_cast<int>(rect.width));

            const int top = std::max(y, rect.y);
            const int bottom = std::min(
                    y + static_cast<int>(height),
                    rect.y + static_cast<int>(rect.height));

            return
                (right - left) > 0 &&
                (bottom - top) > 0;
        }

		//////////////////////////////////////////////////////////////////////////////////////

		namespace cairo_helper {

			void simple_rectangle(cairo_t *cr, const rectangle& rect)
			{
				cairo_rectangle(cr, rect.x, rect.y, rect.width, rect.height);
			}

			void rounded_rectangle(cairo_t * cr, const rectangle & rect, const float radius)
			{
				rounded_rectangle(cr, static_cast<float>(rect.x), static_cast<float>(rect.y), 
					static_cast<float>(rect.width), static_cast<float>(rect.height), radius);
			}
			
			void rounded_rectangle(cairo_t * cr, const float x, const float y, const float width, const float height, const float radius)
			{
				cairo_new_sub_path(cr);
				cairo_arc(cr, x + width - radius, y + radius, radius, -M_PI_2, 0.0);
				cairo_arc(cr, x + width - radius, y + height - radius, radius, 0.0, M_PI_2);
				cairo_arc(cr, x + radius, y + height - radius, radius, M_PI_2, M_PI);
				cairo_arc(cr, x + radius, y + radius, radius, M_PI, M_PI + M_PI_2);
				cairo_close_path(cr);
			}

			void set_source_color(cairo_t * cr, const color c)
			{
				cairo_set_source_rgba(cr, float_red(c), float_green(c), 
					float_blue(c), float_alpha(c));
			}

			void show_centered_text(cairo_t * cr, const rectangle & rect, const char * text)
			{
				cairo_text_extents_t te;

				cairo_text_extents(cr, text, &te);
				cairo_move_to(cr, rect.x + (rect.width - te.width) / 2,
					rect.y + (rect.height + te.height) / 2);

				cairo_show_text(cr, text);
			}

			void show_centered_text(cairo_t * cr, const rectangle & rect, const std::string & text)
			{
				show_centered_text(cr, rect, text.c_str());
			}

			void show_left_aligned_text(cairo_t * cr, const rectangle & rect, const char * text, const float offset)
			{
				cairo_text_extents_t te;

				cairo_text_extents(cr, text, &te);
				cairo_move_to(cr, rect.x + offset, rect.y + (rect.height + te.height) / 2);

				cairo_show_text(cr, text);
			}

			void show_left_aligned_text(cairo_t * cr, const rectangle & rect, const std::string & text, const float offset)
			{
				show_left_aligned_text(cr, rect, text.c_str(), offset);
			}

			void show_right_aligned_text(cairo_t *cr, const rectangle& rect, const char *text)
			{
				cairo_text_extents_t te;

				cairo_text_extents(cr, text, &te);
				cairo_move_to(cr, rect.x + rect.width - te.width,
					rect.y + (rect.height + te.height) / 2);

				cairo_show_text(cr, text);
			}

			void show_right_aligned_text(cairo_t *cr, const rectangle& rect, const std::string& text)
			{
				show_right_aligned_text(cr, rect, text.c_str());
			}

			void circle(cairo_t * cr, const float center_x, const float center_y, const float radius)
			{
				cairo_arc(cr, center_x, center_y, radius, 0.0, 2.0 * M_PI);
			}

			void centered_circle(cairo_t * cr, const rectangle & rect, const float radius)
			{
				const float cx = static_cast<float>(rect.x) + static_cast<float>(rect.width) / 2.0f;
				const float cy = static_cast<float>(rect.y) + static_cast<float>(rect.height) / 2.0f;
				circle(cr, cx, cy, radius);
			}

		}




	} /* View */

} /* Gammou */
