#include "color.h"


namespace Gammou {

	namespace View {
		
		unsigned char uchar_red(const color c)
		{
			return c >> 24;
		}

		unsigned char uchar_green(const color c)
		{
			return (c >> 16) & 0xFF;
		}

		unsigned char uchar_blue(const color c)
		{
			return (c >> 8) & 0xFF;
		}

		unsigned char uchar_alpha(const color c)
		{
			return c & 0xFF;
		}

		float float_red(const color c)
		{
			return static_cast<float>(uchar_red(c)) / 255.0f;
		}

		float float_green(const color c)
		{
			return static_cast<float>(uchar_green(c)) / 255.0f;
		}

		float float_blue(const color c)
		{
			return static_cast<float>(uchar_blue(c)) / 255.0f;
		}

		float float_alpha(const color c)
		{
			return static_cast<float>(uchar_alpha(c)) / 255.0f;
		}

	} /* View */

} /* Gammou */