
cmake_minimum_required (VERSION 3.1)

#######################################################
project(View LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 11)

option(VIEW_APP "Provide Application Window class" OFF)
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
	widget/event.h
	widget/list_box.h
	widget/list_box.cpp
	widget/page_container.h
	widget/page_container.cpp
	widget/panel.h
	widget/panel.cpp
	widget/scrollable_panel.h
	widget/widget.h
	widget/widget.cpp
)

add_library(view STATIC 
	view.h
	cairo_definition.h
	abstract_window.h
	abstract_window.cpp
	${WIDGET_SRC}
)

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake_modules")
find_package(Cairo REQUIRED)

target_include_directories(view PUBLIC ${Cairo_INCLUDE_DIRS})	#	PUBLIC : Users will have to draw with cairo
target_link_libraries(view PUBLIC ${Cairo_LIBS} )

#######################################################
#	Specific Setup

if(WIN32)                  # MICROSOFT WINDOWS

	if(VIEW_VST3)
		#	Add Vst3 win32 window files
		target_include_directories(view PRIVATE ${VST3_SDK_DIR})
		target_sources(view PRIVATE 
			vst3_window/abstract_vst3_window.cpp
			vst3_window/win32/win32_vst3_window.cpp
			${VST3_SDK_DIR}/public.sdk/source/common/pluginview.cpp
		)
    endif()
	
	if(VIEW_APP)
		message( FATAL_ERROR "Not implemented" )
	endif()
	
    
elseif(UNIX)  # LINUX and OSX
    
    if(VIEW_APP)
        find_package(X11 REQUIRED)
        find_package(Xext REQUIRED)
		target_sources(view PRIVATE 
			app_window/X11/x11_app_window.cpp
		)
		target_include_directories(view PUBLIC ${X11_INCLUDE_DIR} ${Xext_INCLUDE_DIR})  
		target_link_libraries(view PRIVATE ${X11_LIBRARIES} ${Xext_LIBRARY})
    endif()

    if(VIEW_VST3)
		message( FATAL_ERROR "Not implemented" )
    endif()

else()
    message( FATAL_ERROR "unsupported OS" )
endif()
    

