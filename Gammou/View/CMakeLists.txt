
cmake_minimum_required (VERSION 3.1)

#######################################################

# Guard

if(TARGET view)
	return()
endif()

#######################################################
project(View LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)

option(VIEW_VST3 "Provide VST3 Window class" OFF)

#######################################################
#	Generic Setup

set(WIDGET_SRC
	widget/cairo_helper.h
	widget/cairo_helper.cpp
	widget/color.h
	widget/color.cpp
	widget/common_widgets.h
	widget/common_widgets.cpp
	widget/dialog.h
	widget/dialog.cpp
	widget/directory_model.h 
	widget/directory_view.h
	#widget/edit_panel.h
	#widget/edit_panel.cpp
	widget/event.h
	widget/list_box.h
	widget/list_box.cpp
	widget/page_container.h
	widget/page_container.cpp
	widget/panel.h
	widget/panel.cpp
	widget/proxy_widget.h
	widget/proxy_widget.cpp
	widget/scrollable_panel.h
	widget/window_widget.h
	widget/window_widget.cpp
	widget/widget.h
	widget/widget.cpp
)

add_library(view STATIC 
	view.h
	cairo_definition.h
	display/abstract_display.h
	display/abstract_display.cpp
	${WIDGET_SRC}
)

#	utility/utility.h
#	utility/utility.cpp

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake_modules")
find_package(Cairo REQUIRED)

target_include_directories(view PUBLIC ${Cairo_INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR})	
target_link_libraries(view PUBLIC ${Cairo_LIBS})

#######################################################
#	Specific Setup

if (WIN32)
	target_sources(view PRIVATE
		display/abstract_win32_display.h
	    display/abstract_win32_display.cpp
	)
	
elseif(UNIX)  # LINUX and OSX
	find_package(X11 REQUIRED)
        find_package(Xext REQUIRED)
	
	target_sources(view PRIVATE
		display/abstract_x11_display.h
	    display/abstract_x11_display.cpp
	)
	
	target_include_directories(view PUBLIC 
		${X11_INCLUDE_DIR} 
		${Xext_INCLUDE_DIR}
	)  
		
	target_link_libraries(view PRIVATE 
		${X11_LIBRARIES} 
		${Xext_LIBRARY}
		stdc++fs			# filesystem
    pthread
	)
elseif (NOT WIN32)
    message( FATAL_ERROR "unsupported OS" )
endif()


#	VST3 Display

if(VIEW_VST3)
	set_target_properties(view PROPERTIES POSITION_INDEPENDENT_CODE ON)  # Allow Modules objects to be built
	target_include_directories(view PRIVATE ${VST3_SDK_DIR})
	target_sources(view PRIVATE 
		display/abstract_vst3_display.h
		display/abstract_vst3_display.cpp
		${VST3_SDK_DIR}/public.sdk/source/common/pluginview.cpp
	)
	
	if(WIN32)
		target_sources(view PRIVATE
			display_implementation/win32_vst3_display.h
			display_implementation/win32_vst3_display.cpp
	)
	elseif(UNIX)
		target_sources(view PRIVATE
			display_implementation/x11_vst3_display.h
			display_implementation/x11_vst3_display.cpp
		)
	endif()
	
endif()

#	VST2 Display

if(VIEW_VST2)
	set_target_properties(view PROPERTIES POSITION_INDEPENDENT_CODE ON)  # Allow Modules objects to be built
	target_sources(view PRIVATE 
		display/abstract_vst2_display.h
		display/abstract_vst2_display.cpp
	)
	
	if(WIN32)
		target_sources(view PRIVATE
			display_implementation/win32_vst2_display.h
			display_implementation/win32_vst2_display.cpp
	)
	elseif(UNIX)
		target_sources(view PRIVATE
			display_implementation/x11_vst2_display.h
			display_implementation/x11_vst2_display.cpp
		)
	endif()
	
endif()

#	Classic display
	
target_sources(view PRIVATE
	display/abstract_application_display.h
    display/abstract_application_display.cpp
)
	
if(WIN32)
	target_sources(view PRIVATE
		display_implementation/win32_application_display.h
		display_implementation/win32_application_display.cpp
	)
elseif(UNIX)
	target_sources(view PRIVATE
		display_implementation/x11_application_display.h
		display_implementation/x11_application_display.cpp
	)
endif()

	


    

