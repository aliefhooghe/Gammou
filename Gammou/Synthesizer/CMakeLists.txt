
cmake_minimum_required (VERSION 3.1)

#######################################################
project(View LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 11 REQUIRED)
 
file(
    GLOB_RECURSE
    SYNTHESIZER_SRC
    *.cpp
)
 
add_library(synthesizer STATIC ${SYNTHESIZER_SRC})
set(PROCESS_INCLUDE_DIR "../Process")

target_include_directories(synthesizer PUBLIC ${PROCESS_INCLUDE_DIR})

           
if(UNIX)  # LINUX and APPLE OSX
	target_link_libraries(synthesizer PRIVATE m dl)	#	math.h and dlopen
endif()
