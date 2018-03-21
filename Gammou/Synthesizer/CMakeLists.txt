cmake_minimum_required (VERSION 3.1)

project (Synthesizer)

set(CMAKE_CXX_STANDARD 11)
 
set(DYNAMIC_LOADING_LIB "") # Os dependant
 
if(WIN_32)                  # MICROSOFT WINDOWS
    # something to add ?         
elseif(UNIX)  # LINUX and APPLE OSX
    set(DYNAMIC_LOADING_LIB "dl") # dlopen, ...
else()
    message( FATAL_ERROR "unsupported os" )
endif()
    
file(
    GLOB_RECURSE
    SYNTHESIZER_SRC
    *.cpp
)

set(PROCESS_INCLUDE_DIR "../Process")

add_library(synthesizer STATIC ${SYNTHESIZER_SRC})
target_include_directories(synthesizer PRIVATE ${PROCESS_INCLUDE_DIR})
target_link_libraries(synthesizer ${DYNAMIC_LOADING_LIB} m) # m =  standard math lib
