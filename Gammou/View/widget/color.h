#ifndef GAMMOU_VIEW_COLOR_H_
#define GAMMOU_VIEW_COLOR_H_

namespace Gammou {

	namespace View {

		typedef unsigned int color;		//	rgba

		color create_color(const unsigned char r, const unsigned char g, const unsigned char b, const unsigned char a);
		color create_color(const unsigned char r, const unsigned char g, const unsigned char b);
		color create_color(const float r, const float g, const float b, const float a);
		color create_color(const float r, const float g, const float b);

		unsigned char uchar_red(const color c);
		unsigned char uchar_green(const color c);
		unsigned char uchar_blue(const color c);
		unsigned char uchar_alpha(const color c);

		float float_red(const color c);
		float float_green(const color c);
		float float_blue(const color c);
		float float_alpha(const color c);

		//////

		enum : color {
			cl_white = 0xFFFFFFFF,
			cl_black = 0x000000FF
		};


	} /* View */

} /* Gammou */


#endif
