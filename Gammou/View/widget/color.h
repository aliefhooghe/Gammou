#ifndef GAMMOU_VIEW_COLOR_H_
#define GAMMOU_VIEW_COLOR_H_

namespace Gammou {

	namespace View {

		typedef unsigned int color;		//	rgba

		unsigned char uchar_red(const color c);
		unsigned char uchar_green(const color c);
		unsigned char uchar_blue(const color c);
		unsigned char uchar_alpha(const color c);

		float float_red(const color c);
		float float_green(const color c);
		float float_blue(const color c);
		float float_alpha(const color c);

	} /* View */

} /* Gammou */


#endif
