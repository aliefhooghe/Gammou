cmake_minimum_required (VERSION 3.1)

project (View)

set(CMAKE_CXX_STANDARD 11)

option(VIEW_APP "Provide Application Window class" ON)
option(VIEW_VST3 "Provide VST3 Window class" OFF)

set(WINDOW_SRC abstract_window.cpp)
set(WINDOW_SYSTEM_LIBS "")  # OS dependant window management libs


if(WIN_32)                  # MICROSOFT WINDOWS
     
    message( FATAL_ERROR "Not implemented" )
    
elseif(UNIX AND NOT APPLE)  # LINUX
    
    if(VIEW_APP)
        set(WINDOW_SYSTEM_LIB "X11")
        set(WINDOW_SRC ${WINDOW_SRC} app_window/X11/x11_app_window.cpp)
    endif()

    if(VIEW_VST3)
        message( FATAL_ERROR "Not implemented" )
    endif()

else()
#elseif(APPLE)           # APPLE OSX
    message( FATAL_ERROR "unsupported os" )
endif()
    

file(
    GLOB_RECURSE
    WIDGET_SRC
    widget/*.cpp
)

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake_modules")
find_package(Cairo REQUIRED)

add_library(view STATIC ${WINDOW_SRC} ${WIDGET_SRC})

target_include_directories(view PRIVATE ${Cairo_INCLUDE_DIRS})
target_link_libraries(view PRIVATE ${Cairo_LIBS} ${WINDOW_SYSTEM_LIB})


