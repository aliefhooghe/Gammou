#ifndef CAIRO_HELPER_H_
#define CAIRO_HELPER_H_

#include "../cairo_definition.h"

namespace Gammou {

	namespace View {

		struct color {

			color(const float vr, const float vg, const float vb, const float va);
			color(const float vr, const float vg, const float vb);
			
			float r;
			float g;
			float b;
			float a;
		};

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

		void cairo_rounded_rectangle(cairo_t *cr, const float x, const float y, const float width, const float height, const float radius);

	} /* View */

} /* Gammou */





#endif