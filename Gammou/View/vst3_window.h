#ifndef GAMMOU_VST3_WINDOW_H_
#define GAMMOU_VST3_WINDOW_H_

#ifdef WIN32
#include "gammou_win32_vst3_window.h"
#endif


namespace Gammou {

	namespace View {
#ifdef WIN32
		typedef win32_vst3_window vst3_window;
#endif
	} /* View */

} /* Gammou */



#endif