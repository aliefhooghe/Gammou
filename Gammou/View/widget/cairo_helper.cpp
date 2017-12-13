#define  _USE_MATH_DEFINES
#include <cmath>

#include "cairo_helper.h"

namespace Gammou {

	namespace View {



		rectangle::rectangle(const int px, const int py, const int width, const int height)
			: x(px), y(py), width(width), height(height)
		{
		}

		rectangle rectangle::translate(const int px, const int py) const
		{
			return rectangle(x + px, y + py, width, height);
		}

		bool rectangle::contains(const int px, const int py) const
		{
			return (px >= x && px < (x + (int)width))
				&& (py >= y && py < (y + (int)height));
		}

		bool rectangle::contains(const rectangle & rect) const
		{
			return contains(rect.x, rect.y)
				&& contains(rect.x, rect.y + rect.height - 1)
				&& contains(rect.x + rect.width - 1, rect.y + rect.height - 1)
				&& contains(rect.x + rect.width - 1, rect.y);
		}





		//////////////////////////////////////////////////////////////////////////////////////


		namespace cairo_helper {

			void rounded_rectangle(cairo_t * cr, const rectangle & rect, const float radius)
			{
				rounded_rectangle(cr, rect.x, rect.y, rect.width, rect.height, radius);
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

		}




	} /* View */

} /* Gammou */