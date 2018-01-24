#ifndef CAIRO_HELPER_H_
#define CAIRO_HELPER_H_

#include <string>
#include "../cairo_definition.h"
#include "color.h"

namespace Gammou {

	namespace View {


		struct rectangle {

			rectangle(const int px, const int py, const int width, const int height);
			~rectangle() {};

			rectangle translate(const int px, const int py) const;
			bool contains(const int px, const int py) const;
			bool contains(const rectangle& rect) const;

			int x;
			int y;
			unsigned int width;
			unsigned int height;
		};


		//////////////////////////////////////////////////////////////////////////////////////


		namespace cairo_helper {
			
			void rounded_rectangle(cairo_t *cr, const rectangle& rect, const float radius);
			void rounded_rectangle(cairo_t *cr, const float x, const float y, const float width, const float height, const float radius);
			
			void set_source_color(cairo_t *cr, const color c);

			void show_centered_text(cairo_t *cr, const rectangle& rect, const char *text);
			void show_centered_text(cairo_t *cr, const rectangle& rect, const std::string& text);

			void show_left_aligned_text(cairo_t *cr, const rectangle& rect, const char *text);
			void show_left_aligned_text(cairo_t *cr, const rectangle& rect, const std::string& text);

			void show_right_aligned_text(cairo_t *cr, const rectangle& rect, const char *text);
			void show_right_aligned_text(cairo_t *cr, const rectangle& rect, const std::string& text);

			void circle(cairo_t *cr, const float center_x, const float center_y, const float radius);
			void centered_circle(cairo_t *cr, const rectangle& rect, const float radius);

		}	/* cairo_helper */

		

	} /* View */

} /* Gammou */





#endif