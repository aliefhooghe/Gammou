cmake_minimum_required (VERSION 3.1)

project (View)

set(CMAKE_CXX_STANDARD 11)

option(VIEW_APP "Provide Application Window class" ON)
option(VIEW_VST3 "Provide VST3 Window class" OFF)




set(WINDOW_SRC abstract_window.cpp)

if(WIN_32)                  # MICROSOFT WINDOWS
     
    message( FATAL_ERROR "Not implemented" )
    
elseif(UNIX AND NOT APPLE)  # LINUX
    
    if(VIEW_APP)
        set(WINDOW_SRC ${WINDOW_SRC} app_window/X11/x11_app_window.cpp)
    endif(VIEW_APP)

    if(VIEW_VST3)
        message( FATAL_ERROR "Not implemented" )
    endif(VIEW_VST3)

else(WIN_32)
#elseif(APPLE)           # APPLE OSX
    message( FATAL_ERROR "unsupported os" )
endif(WIN_32)
    


file(
    GLOB_RECURSE
    WIDGET_SRC
    widget/*.cpp
)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake_modules")
find_package(Cairo REQUIRED)

add_library(view STATIC ${WINDOW_SRC} ${WIDGET_SRC})

target_include_directories(view PRIVATE ${Cairo_INCLUDE_DIRS})
target_link_libraries(view PRIVATE ${Cairo_LIBS})

