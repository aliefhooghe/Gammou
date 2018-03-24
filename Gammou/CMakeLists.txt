cmake_minimum_required (VERSION 3.1)

project (Gammou)

# Vs Code 
include(CMakeToolsHelpers OPTIONAL)

set(CMAKE_CXX_STANDARD 11)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pg")
SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -pg")
SET(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -pg")

# Only linux standalone app now

option(STANDALONE_BUILD "Build a Standalone Application" ON)
option(VST3_BUILD "Build a VST3 Plugin" OFF)


macro(SET_OPTION opt value)
  set(${opt} ${value} CACHE INTERNAL "" FORCE)
endmacro()

if(STANDALONE_BUILD)
    SET_OPTION(VIEW_APP ON)
endif()

if(VST3_BUILD)
    #SET_OPTION(VIEW_VST3 ON)
    message( FATAL_ERROR "Not implemented" )
endif()


set(LIBRARY_OUTPUT_PATH "${CMAKE_BINARY_DIR}/lib")
set(EXECUTABLE_OUTPUT_PATH "${CMAKE_BINARY_DIR}/bin")

# BUild Gammou View lib
add_subdirectory(View View)

# Build Gammou Synthesizer lib
add_subdirectory(Synthesizer Synthesizer)


if(WIN_32)                  # MICROSOFT WINDOWS
     
    message( FATAL_ERROR "Not implemented" )
    
elseif(UNIX AND NOT APPLE)  # LINUX
    
    if(STANDALONE_BUILD)
        file(
            GLOB_RECURSE
            STANDALONE_SRC
            Application/standalone/X11/*.cpp
        )
    endif()
    
    if(VST3_BUILD)
        message( FATAL_ERROR "Not implemented" )
    endif()
    

else()
#elseif(APPLE)           # APPLE OSX
    message( FATAL_ERROR "unsupported os" )
endif()


file(
    GLOB_RECURSE
    GUI_SRC
    Application/gui/*.cpp
)

file(
    GLOB_RECURSE
    PERSISTENCE_SRC
    Application/persistence/*.cpp
)


set(VIEW_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/View")
set(PROCESS_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/Process")
set(SYNTHESIZER_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/Synthesizer")
set(GUI_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/Application/gui")

set(RTAUDIO_DIR "${CMAKE_SOURCE_DIR}/../rtaudio")

if(STANDALONE_BUILD)
    add_executable(gammou_standalone ${STANDALONE_SRC} ${GUI_SRC} ${PERSISTENCE_SRC} "${RTAUDIO_DIR}/RtAudio.cpp")
    target_include_directories(gammou_standalone PRIVATE ${VIEW_INCLUDE_DIR} ${PROCESS_INCLUDE_DIR} ${SYNTHESIZER_INCLUDE_DIR} ${GUI_INCLUDE_DIR} ${Cairo_INCLUDE_DIRS} ${RTAUDIO_DIR})
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread") # not Ok
    target_link_libraries(gammou_standalone PRIVATE ${Cairo_LIBS} view synthesizer pulse-simple pulse m)
    target_compile_definitions(gammou_standalone PRIVATE GAMMOU_VIEW_APP __LINUX_PULSE__)
endif()
    
if(VST3_BUILD)
    message( FATAL_ERROR "Not implemented" )
endif()


