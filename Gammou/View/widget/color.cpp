#include "color.h"


namespace Gammou {

	namespace View {

		color create_color(const unsigned char r, const unsigned char g, const unsigned char b, const unsigned char a)
		{
			return (r << 24) | (g << 16) | (b << 8) | a;
		}

		color create_color(const unsigned char r, const unsigned char g, const unsigned char b)
		{
			return create_color(r, g, b, 255u);
		}

		color create_color(const float r, const float g, const float b, const float a)
		{
			const unsigned char ur = static_cast<unsigned char>(r * 255.0f);
			const unsigned char ug = static_cast<unsigned char>(g * 255.0f);
			const unsigned char ub = static_cast<unsigned char>(b * 255.0f);
			const unsigned char ua = static_cast<unsigned char>(a * 255.0f);
			return create_color(ur, ug, ub, ua);
		}

		color create_color(const float r, const float g, const float b)
		{
			return create_color(r, g, b, 1.0f);
		}

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