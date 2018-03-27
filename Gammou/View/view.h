#ifndef VIEW_H_
#define VIEW_H_

#include "../debug.h" // a enlever a terme

#include "cairo_definition.h"
#include "widget/event.h"
#include "widget/widget.h"
#include "widget/panel.h"
#include "widget/page_container.h"
#include "widget/scrollable_panel.h"
#include "widget/list_box.h"
#include "abstract_window.h"

#include "widget/common_widgets.h"


#if !defined(GAMMOU_VIEW_APP) && !defined(GAMMOU_VIEW_VST3)
#error "Gammou : View : You must define at least one of he option : GAMMOU_VIEW_APP or GAMMOU_VIEW_VST3"
#endif


//----------------------------------------------------------------------
#ifdef __linux__

#ifdef GAMMOU_VIEW_VST3
#error "Gammou : View : Not implemented"
#endif

#ifdef GAMMOU_VIEW_APP
#include "app_window/X11/x11_app_window.h"
#endif



//----------------------------------------------------------------------
#elif defined(_WIN32)

#ifdef GAMMOU_VIEW_VST3
#include "vst3_window\win32\win32_vst3_window.h"
#endif

#ifdef GAMMOU_VIEW_APP
#error "Gammou : View : Not implemented"
#endif




//----------------------------------------------------------------------
#elif defined(__APPLE__)
//----------------------------------------------------------------------

#ifdef GAMMOU_VIEW_VST3
#error "Gammou : View : Not implemented"
#endif

#ifdef GAMMOU_VIEW_APP
#include "app_window/X11/x11_app_window.h"
#endif

//----------------------------------------------------------------------
#else
#error "Gammou : View : Operating System Not Supported"
#endif


namespace Gammou {

	namespace View {

//--------------------

//  Special Window

#if defined(__linux__) || defined(__APPLE__)

#ifdef GAMMOU_VIEW_VST3
		#error "Not Implemented"
#elif defined GAMMOU_VIEW_APP
		typedef x11_app_window generic_app_window;
#endif

#elif defined _WIN32

#ifdef GAMMOU_VIEW_VST3
		typedef win32_vst3_window generic_vst3_window;
#elif GAMMOU_VIEW_APP
		// typedef *** generic_app_window
#error "Not Implemented"
#endif

#endif
//--------------------

// Generic window (Only if one target is specified)

#if defined(GAMMOU_VIEW_VST3) && !defined(GAMMOU_VIEW_APP)
		typedef generic_vst3_window generic_window;
#elif  defined(GAMMOU_VIEW_APP) && !defined(GAMMOU_VIEW_VST3)
		typedef generic_app_window generic_window;
#endif
    
	}

}


#endif
