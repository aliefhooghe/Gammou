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
#include "widget/dialog.h"

//----------------------------------------------------------------------
#ifdef __linux__

#ifdef GAMMOU_VIEW_VST3
#include "display_implementation/x11_vst3_display.h"
#endif

#ifdef GAMMOU_VIEW_VST2
#include "display_implementation/x11_vst2_display.h"
#endif

#include "display_implementation/x11_application_display.h"

//----------------------------------------------------------------------
#elif defined(_WIN32)

#ifdef GAMMOU_VIEW_VST3
#include "display_implementation/win32_vst3_display.h"
#endif

#ifdef GAMMOU_VIEW_VST2
//#include "display_implementation/win32_vst2_display.h"
#error "Gammou : View : Not implemented"
#endif

#include "display_implementation/win32_application_display.h"

//----------------------------------------------------------------------
#elif defined(__APPLE__)

#ifdef GAMMOU_VIEW_VST3
#error "Gammou : View : Not implemented"
#endif

#ifdef GAMMOU_VIEW_VST2
#error "Gammou : View : Not implemented"
#endif

#include "display_implementation/x11_application_display.h"


//----------------------------------------------------------------------
#else
#error "Gammou : View : Operating System Not Supported"
#endif


namespace Gammou {

	namespace View {

//--------------------

//  Special Displays

#if defined(__linux__)
//----------------------------------------------------------
	typedef x11_application_display application_display;
	typedef application_display dialog_display;

#ifdef GAMMOU_VIEW_VST3
		typedef x11_vst3_display vst3_display;
#endif

#ifdef GAMMOU_VIEW_VST2
		typedef x11_vst2_display vst2_display;
#endif
//----------------------------------------------------------
#elif defined _WIN32
//----------------------------------------------------------
	typedef win32_application_display application_display;
	typedef application_display dialog_display;

#ifdef GAMMOU_VIEW_VST3
		typedef win32_vst3_display vst3_display;
#endif

#ifdef GAMMOU_VIEW_VST2
		typedef win32_vst2_display vst2_display;
#endif
		
//----------------------------------------------------------
#endif
//--------------------

	}

}


#endif
