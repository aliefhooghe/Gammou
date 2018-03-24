cmake_minimum_required (VERSION 3.1)


# Macro for setting option in subproject
macro(SET_OPTION opt value)
  set(${opt} ${value} CACHE INTERNAL "" FORCE)
endmacro()


#################################################

project (Gammou)

option(STANDALONE_BUILD "Build a Standalone Application" ON)
option(VST3_BUILD "Build a VST3 Plugin" OFF)

# Use C++ 11
set(CMAKE_CXX_STANDARD 11)

# Set output build paths
set(LIBRARY_OUTPUT_PATH "${CMAKE_BINARY_DIR}/lib")
set(EXECUTABLE_OUTPUT_PATH "${CMAKE_BINARY_DIR}/bin")

#################################################

set(STANDALONE_APPLICATION_SOUND_SYSTEM_LIBS "") # OS dependant sound libs
set(STANDALONE_APPLICATION_DEFINITION "")        # OS dependant additional definitions

#################################################

if(STANDALONE_BUILD)
    SET_OPTION(VIEW_APP ON) # Enable Application Window in View
endif()

if(VST3_BUILD)
    #SET_OPTION(VIEW_VST3 ON)   # Enable VST3 Window in VIew
    message( FATAL_ERROR "Not implemented" )
endif()


# Build Gammou View lib
add_subdirectory(View View)

# Build Gammou Synthesizer lib
add_subdirectory(Synthesizer Synthesizer)



if(WIN_32)                  # MICROSOFT WINDOWS
     
    message( FATAL_ERROR "Not implemented" )
    
elseif(UNIX AND NOT APPLE)  # LINUX
    
    if(STANDALONE_BUILD)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
        set(STANDALONE_APPLICATION_SOUND_SYSTEM_LIBS pulse-simple pulse)
        set(STANDALONE_APPLICATION_DEFINITION __LINUX_PULSE__)
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
    target_link_libraries(gammou_standalone PRIVATE ${Cairo_LIBS} ${STANDALONE_APPLICATION_SOUND_SYSTEM_LIBS} view synthesizer m)
    target_compile_definitions(gammou_standalone PRIVATE GAMMOU_VIEW_APP ${STANDALONE_APPLICATION_DEFINITION})
endif()
    
if(VST3_BUILD)
    message( FATAL_ERROR "Not implemented" )
endif()


