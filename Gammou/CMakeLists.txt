cmake_minimum_required (VERSION 3.1)


# Macro for setting option in subproject
macro(SET_OPTION opt value)
  set(${opt} ${value} CACHE INTERNAL "" FORCE)
endmacro()


#################################################

project (Gammou LANGUAGES CXX)

option(STANDALONE_BUILD "Build a Standalone Application" OFF)
option(VST3_BUILD "Build a VST3 Plugin" ON)

# Use C++ 11
set(CMAKE_CXX_STANDARD 11)

# Set output build paths
set(LIBRARY_OUTPUT_PATH "${CMAKE_BINARY_DIR}/lib")
set(EXECUTABLE_OUTPUT_PATH "${CMAKE_BINARY_DIR}/bin")

#################################################

set(STANDALONE_APPLICATION_SOUND_SYSTEM_LIBS "") # OS dependant sound libs
set(STANDALONE_APPLICATION_DEFINITION "")        # OS dependant additional definitions

#################################################

set(VIEW_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/View")
set(SYNTHESIZER_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/Synthesizer")
set(GUI_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/Application/gui")

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

if(STANDALONE_BUILD)
    SET_OPTION(VIEW_APP ON) # Enable Application Window in View
	set(RTAUDIO_DIR "${CMAKE_SOURCE_DIR}/../rtaudio")
	
    add_executable(gammou_standalone ${GUI_SRC} ${PERSISTENCE_SRC} "${RTAUDIO_DIR}/RtAudio.cpp")
	target_include_directories(gammou_standalone PRIVATE ${VIEW_INCLUDE_DIR} ${SYNTHESIZER_INCLUDE_DIR} ${GUI_INCLUDE_DIR} ${RTAUDIO_DIR})
	target_link_libraries(gammou_standalone PRIVATE ${STANDALONE_APPLICATION_SOUND_SYSTEM_LIBS} view synthesizer)
	target_compile_definitions(gammou_standalone PRIVATE GAMMOU_VIEW_APP)
endif()

if(VST3_BUILD)
    SET_OPTION(VIEW_VST3 ON)   # Enable VST3 Window in View
	
	set(VST3_SDK_DIR "${CMAKE_SOURCE_DIR}/../VST_SDK/VST3_SDK")	
	add_subdirectory(${VST3_SDK_DIR} VST3_SDK EXCLUDE_FROM_ALL)	# only need base and sdk targets
	
	add_library(gammou MODULE 
		${VST3_SDK_DIR}/public.sdk/source/vst/vstsinglecomponenteffect.cpp
		Application/vst3/factory.cpp
		Application/vst3/vst3_plugin.cpp
		${VST3_PLUGIN_SRC} ${GUI_SRC} ${PERSISTENCE_SRC}
	)
	
	target_include_directories(gammou PRIVATE ${VST3_SDK_DIR} ${VIEW_INCLUDE_DIR} ${SYNTHESIZER_INCLUDE_DIR} ${GUI_INCLUDE_DIR})
    target_link_libraries(gammou PRIVATE view synthesizer base sdk)	# base and sdk from VST3_SDK
	target_compile_definitions(gammou PRIVATE GAMMOU_VIEW_VST3)
	set_target_properties(gammou PROPERTIES LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
endif()

#################################################

# Build Gammou View lib
add_subdirectory(View View)

# Build Gammou Synthesizer lib
add_subdirectory(Synthesizer Synthesizer)

if(WIN32)                  # MICROSOFT WINDOWS
     
	if(STANDALONE_BUILD)
		message( FATAL_ERROR "Not implemented" )
	endif()
	
	if(VST3_BUILD)
		target_sources(gammou PRIVATE "${VST3_SDK_DIR}/public.sdk/source/main/dllmain.cpp")
		set_target_properties(gammou PROPERTIES SUFFIX ".vst3")
		set_target_properties(gammou PROPERTIES LINK_FLAGS "/EXPORT:GetPluginFactory")
	endif()
    
    
elseif(UNIX AND NOT APPLE)  # LINUX
    
    if(STANDALONE_BUILD)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
		target_link_libraries(gammou_standalone PRIVATE m pulse-simple pulse)
        target_compile_definitions(gammou_standalone __LINUX_PULSE__)
        target_source(gammou_standalone PRIVATE Application/standalone/X11/main.cpp)       
    endif()
    
    if(VST3_BUILD)
        message( FATAL_ERROR "Not implemented" )
    endif()
    
else()
#elseif(APPLE)           # APPLE OSX
    message( FATAL_ERROR "unsupported os" )
endif()



