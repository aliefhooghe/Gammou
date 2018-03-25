
cmake_minimum_required (VERSION 3.1)

#######################################################
project(View LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 11 REQUIRED)

option(VIEW_APP "Provide Application Window class" OFF)
option(VIEW_VST3 "Provide VST3 Window class" OFF)

#######################################################
#	Generic Setup

file(
    GLOB_RECURSE
    WIDGET_SRC
    widget/*.cpp
)

add_library(view STATIC 
	abstract_window.cpp
	${WIDGET_SRC}
)

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake_modules")
find_package(Cairo REQUIRED)

target_include_directories(view PUBLIC ${Cairo_INCLUDE_DIRS})	#	PUBLIC : Users will have to draw with cairo
target_link_libraries(view PRIVATE ${Cairo_LIBS} )

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
	
    
elseif(UNIX AND NOT APPLE)  # LINUX
    
    if(VIEW_APP)
		target_sources(view PRIVATE 
			app_window/X11/x11_app_window.cpp
		)
		target_link_libraries(view PRIVATE X11 Xext)
    endif()

    if(VIEW_VST3)
		message( FATAL_ERROR "Not implemented" )
    endif()

else()
#elseif(APPLE)           # APPLE OSXs
    message( FATAL_ERROR "unsupported os (Apple)" )
endif()
    

