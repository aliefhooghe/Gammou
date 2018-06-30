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
#include "widget/common_widgets.h"
#include "widget/window_widget.h"

#if !defined(GAMMOU_VIEW_APP) && !defined(GAMMOU_VIEW_VST3)
#error "Gammou : View : You must define at least one of he option : GAMMOU_VIEW_APP or GAMMOU_VIEW_VST3"
#endif


//----------------------------------------------------------------------
#ifdef __linux__

#ifdef GAMMOU_VIEW_VST3
#include "display_implementation/x11_vst3_display.h"
#endif

#ifdef GAMMOU_VIEW_APP
#include "display_implementation/x11_application_display.h"
#endif



//----------------------------------------------------------------------
#elif defined(_WIN32)

#ifdef GAMMOU_VIEW_VST3
#include "display_implementation/win32_vst3_display.h"
#endif

#ifdef GAMMOU_VIEW_APP
#include "display_implementation/win32_application_display.h"
#endif




//----------------------------------------------------------------------
#elif defined(__APPLE__)
//----------------------------------------------------------------------

#ifdef GAMMOU_VIEW_VST3
#error "Gammou : View : Not implemented"
#endif

#ifdef GAMMOU_VIEW_APP
#include "display_implementation/x11_application_display.h"
#endif

//----------------------------------------------------------------------
#else
#error "Gammou : View : Operating System Not Supported"
#endif


namespace Gammou {

	namespace View {

//--------------------

//  Special Displays

#if defined(__linux__)

#ifdef GAMMOU_VIEW_APP
		typedef x11_application_display application_display;
#endif

#if defined(GAMMOU_VIEW_VST3)
		typedef x11_vst3_display vst3_display;
#endif

#elif defined _WIN32

#ifdef GAMMOU_VIEW_VST3
		typedef win32_vst3_display vst3_display;
#elif GAMMOU_VIEW_APP
		// typedef win32_application_display application_display;
#error "Not Implemented"
#endif

#endif
//--------------------

// Generic window (Only if one target is specified)

#if defined(GAMMOU_VIEW_VST3) && !defined(GAMMOU_VIEW_APP)
		typedef vst3_display generic_display;
#elif  defined(GAMMOU_VIEW_APP) && !defined(GAMMOU_VIEW_VST3)
		typedef application_display generic_display;
#endif
    
	}

}


#endif
