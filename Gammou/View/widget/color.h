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
			cl_aliceblue = 0xF0F8FFFF,
			cl_antiquewhite = 0xFAEBD7FF,
			cl_aqua = 0x00FFFFFF,
			cl_aquamarine = 0x7FFFD4FF,
			cl_azure = 0xF0FFFFFF,
			cl_beige = 0xF5F5DCFF,
			cl_bisque = 0xFFE4C4FF,
			cl_black = 0x000000FF,
			cl_blanchedalmond = 0xFFEBCDFF,
			cl_blue = 0x0000FFFF,
			cl_blueviolet = 0x8A2BE2FF,
			cl_brown = 0xA52A2AFF,
			cl_burlywood = 0xDEB887FF,
			cl_cadetblue = 0x5F9EA0FF,
			cl_chartreuse = 0x7FFF00FF,
			cl_chocolate = 0xD2691EFF,
			cl_coral = 0xFF7F50FF,
			cl_cornflowerblue = 0x6495EDFF,
			cl_cornsilk = 0xFFF8DCFF,
			cl_crimson = 0xDC143CFF,
			cl_cyan = 0x00FFFFFF,
			cl_darkblue = 0x00008BFF,
			cl_darkcyan = 0x008B8BFF,
			cl_darkgoldenrod = 0xB8860BFF,
			cl_darkgray = 0xA9A9A9FF,
			cl_darkgreen = 0x006400FF,
			cl_darkkhaki = 0xBDB76BFF,
			cl_darkmagenta = 0x8B008BFF,
			cl_darkolivegreen = 0x556B2FFF,
			cl_darkorange = 0xFF8C00FF,
			cl_darkorchid = 0x9932CCFF,
			cl_darkred = 0x8B0000FF,
			cl_darksalmon = 0xE9967AFF,
			cl_darkseagreen = 0x8FBC8FFF,
			cl_darkslateblue = 0x483D8BFF,
			cl_darkslategray = 0x2F4F4FFF,
			cl_darkturquoise = 0x00CED1FF,
			cl_darkviolet = 0x9400D3FF,
			cl_deeppink = 0xFF1493FF,
			cl_deepskyblue = 0x00BFFFFF,
			cl_dimgray = 0x696969FF,
			cl_dodgerblue = 0x1E90FFFF,
			cl_firebrick = 0xB22222FF,
			cl_floralwhite = 0xFFFAF0FF,
			cl_forestgreen = 0x228B22FF,
			cl_fuchsia = 0xFF00FFFF,
			cl_gainsboro = 0xDCDCDCFF,
			cl_ghostwhite = 0xF8F8FFFF,
			cl_gold = 0xFFD700FF,
			cl_goldenrod = 0xDAA520FF,
			cl_gray = 0x808080FF,
			cl_green = 0x008000FF,
			cl_greenyellow = 0xADFF2FFF,
			cl_honeydew = 0xF0FFF0FF,
			cl_hotpink = 0xFF69B4FF,
			cl_indianred = 0xCD5C5CFF,
			cl_indigo = 0x4B0082FF,
			cl_ivory = 0xFFFFF0FF,
			cl_khaki = 0xF0E68CFF,
			cl_lavender = 0xE6E6FAFF,
			cl_lavenderblush = 0xFFF0F5FF,
			cl_lawngreen = 0x7CFC00FF,
			cl_lemonchiffon = 0xFFFACDFF,
			cl_lightblue = 0xADD8E6FF,
			cl_lightcoral = 0xF08080FF,
			cl_lightcyan = 0xE0FFFFFF,
			cl_lightgoldenrodyellow = 0xFAFAD2FF,
			cl_lightgreen = 0x90EE90FF,
			cl_lightgrey = 0xD3D3D3FF,
			cl_lightpink = 0xFFB6C1FF,
			cl_lightsalmon = 0xFFA07AFF,
			cl_lightseagreen = 0x20B2AAFF,
			cl_lightskyblue = 0x87CEFAFF,
			cl_lightslategray = 0x778899FF,
			cl_lightsteelblue = 0xB0C4DEFF,
			cl_lightyellow = 0xFFFFE0FF,
			cl_lime = 0x00FF00FF,
			cl_limegreen = 0x32CD32FF,
			cl_linen = 0xFAF0E6FF,
			cl_magenta = 0xFF00FFFF,
			cl_maroon = 0x800000FF,
			cl_mediumaquamarine = 0x66CDAAFF,
			cl_mediumblue = 0x0000CDFF,
			cl_mediumorchid = 0xBA55D3FF,
			cl_mediumpurple = 0x9370DBFF,
			cl_mediumseagreen = 0x3CB371FF,
			cl_mediumslateblue = 0x7B68EEFF,
			cl_mediumspringgreen = 0x00FA9AFF,
			cl_mediumturquoise = 0x48D1CCFF,
			cl_mediumvioletred = 0xC71585FF,
			cl_midnightblue = 0x191970FF,
			cl_mintcream = 0xF5FFFAFF,
			cl_mistyrose = 0xFFE4E1FF,
			cl_moccasin = 0xFFE4B5FF,
			cl_navajowhite = 0xFFDEADFF,
			cl_navy = 0x000080FF,
			cl_oldlace = 0xFDF5E6FF,
			cl_olive = 0x808000FF,
			cl_olivedrab = 0x6B8E23FF,
			cl_orange = 0xFFA500FF,
			cl_orangered = 0xFF4500FF,
			cl_orchid = 0xDA70D6FF,
			cl_palegoldenrod = 0xEEE8AAFF,
			cl_palegreen = 0x98FB98FF,
			cl_paleturquoise = 0xAFEEEEFF,
			cl_palevioletred = 0xDB7093FF,
			cl_papayawhip = 0xFFEFD5FF,
			cl_peachpuff = 0xFFDAB9FF,
			cl_peru = 0xCD853FFF,
			cl_pink = 0xFFC0CBFF,
			cl_plum = 0xDDA0DDFF,
			cl_powderblue = 0xB0E0E6FF,
			cl_purple = 0x800080FF,
			cl_red = 0xFF0000FF,
			cl_rosybrown = 0xBC8F8FFF,
			cl_royalblue = 0x4169E1FF,
			cl_saddlebrown = 0x8B4513FF,
			cl_salmon = 0xFA8072FF,
			cl_sandybrown = 0xF4A460FF,
			cl_seagreen = 0x2E8B57FF,
			cl_seashell = 0xFFF5EEFF,
			cl_sienna = 0xA0522DFF,
			cl_silver = 0xC0C0C0FF,
			cl_skyblue = 0x87CEEBFF,
			cl_slateblue = 0x6A5ACDFF,
			cl_slategray = 0x708090FF,
			cl_snow = 0xFFFAFAFF,
			cl_springgreen = 0x00FF7FFF,
			cl_steelblue = 0x4682B4FF,
			cl_tan = 0xD2B48CFF,
			cl_teal = 0x008080FF,
			cl_thistle = 0xD8BFD8FF,
			cl_tomato = 0xFF6347FF,
			cl_turquoise = 0x40E0D0FF,
			cl_violet = 0xEE82EEFF,
			cl_wheat = 0xF5DEB3FF,
			cl_white = 0xFFFFFFFF,
			cl_whitesmoke = 0xF5F5F5FF,
			cl_yellow = 0xFFFF00FF,
			cl_yellowgreen = 0x9ACD32FF
		};


	} /* View */

} /* Gammou */


#endif
