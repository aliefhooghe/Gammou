#ifndef VIEW_H_
#define VIEW_H_

#include "../debug.h" // a enlever a terme

#include "cairo_definition.h"
#include "widget\widget.h"
#include "widget\panel.h"
#include "widget\page_container.h"
#include "widget\list_box.h"
#include "abstract_window.h"

#include "widget\common_widgets.h"


#if defined GAMMOU_VIEW_APP && defined GAMMOU_VIEW_VST3
#error "Gammou : View : It is impossible to use both vst3 and app window at the same time"
#endif

#ifdef GAMMOU_VIEW_VST3
#include "vst3_window\abstract_vst3_window.h"
#elif GAMMOU_VIEW_APP
#include "app_window\abstract_app_window.h"
#else
#error "Gammou : View : either GAMMOU_VIEW_VST3 or GAMMOU_VIEW_APP must be defined"
#endif

#ifdef __linux__

#ifdef GAMMOU_VIEW_VST3
	// todo
#elif GAMMOU_VIEW_APP
	// todo
#endif

#elif _WIN32

#ifdef GAMMOU_VIEW_VST3
#include "vst3_window\win32\win32_vst3_window.h"
#elif GAMMOU_VIEW_APP
	//	todo
#endif

#else
#error "Gammou : View : Operating System Not Supported"
#endif


namespace Gammou {

	namespace View {

//--------------------
#ifdef __linux__

#ifdef GAMMOU_VIEW_VST3
		// todo
#elif GAMMOU_VIEW_APP
		// todo
#endif

#elif _WIN32

#ifdef GAMMOU_VIEW_VST3
		typedef win32_vst3_window vst3_window;
#elif GAMMOU_VIEW_APP
		// todo
#endif

#endif
//--------------------

#ifdef GAMMOU_VIEW_VST3
		typedef vst3_window generic_window;
#elif GAMMOU_VIEW_APP
		// todo
#endif

	}

}


#endif