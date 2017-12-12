#ifndef GAMMOU_GEOMETRY_H_
#define GAMMOU_GEOMETRY_H_

#include "../cairo_definition.h"

namespace Gammou {

	namespace View {
		/*
		class color {

		public:


			float r;
			float g;
			float b;
			float a;
		};
		*/

		class rectangle {

		public:
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