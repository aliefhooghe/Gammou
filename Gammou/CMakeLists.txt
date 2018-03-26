cmake_minimum_required (VERSION 3.1)


# Macro for setting option in subproject
macro(set_option opt value)
  set(${opt} ${value} CACHE INTERNAL "" FORCE)
endmacro()

# Macro to copy target after build
macro(add_post_build_copy target directory)
		add_custom_command(TARGET ${target} POST_BUILD
			COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${target}> ${directory}
			COMMENT "Copying ${target} to ${directory}"
		)
endmacro()


#################################################

project (Gammou LANGUAGES CXX)

option(GAMMOU_STANDALONE_BUILD "Build a Standalone Application" ON)
option(GAMMOU_VST3_BUILD "Build a VST3 Plugin" OFF)

# Hide View Option
mark_as_advanced(FORCE 
	VIEW_APP
	VIEW_VST
)	

# Use C++ 11
set(CMAKE_CXX_STANDARD 11)

# Enable cmake FOLDER propertie
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

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


set(GUI_SRC
	Application/gui/abstract_gui_component_factory.h
	Application/gui/abstract_gui_component_factory.cpp
	Application/gui/gui_component.cpp
	Application/gui/gui_component.h
	Application/gui/gui_component_main_factory.h
	Application/gui/gui_component_main_factory.cpp
	Application/gui/gui_master_circuit.h
	Application/gui/gui_master_circuit.cpp
	Application/gui/gui_polyphonic_circuit.h
	Application/gui/gui_polyphonic_circuit.cpp
	Application/gui/gui_properties.h
	Application/gui/gui_sound_component.h
	Application/gui/gui_sound_component.cpp
	Application/gui/gui_synthesizer_circuit.h
	Application/gui/gui_synthesizer_circuit.cpp
	Application/gui/synthesizer_gui.h
	Application/gui/synthesizer_gui.cpp
	Application/gui/control_component/knob_component.h
	Application/gui/control_component/knob_component.cpp
)

set(PERSISTENCE_SRC
    Application/persistence/synthesizer_persistence.h
	Application/persistence/synthesizer_persistence.cpp
)

if(GAMMOU_STANDALONE_BUILD)
    set_option(VIEW_APP ON) # Enable Application Window in View
	set(RTAUDIO_DIR "${CMAKE_SOURCE_DIR}/../rtaudio")
	
    add_executable(gammou_standalone 
		${GUI_SRC} ${PERSISTENCE_SRC} 
		"${RTAUDIO_DIR}/RtAudio.cpp"
	)
	
	target_include_directories(gammou_standalone PRIVATE 
		${VIEW_INCLUDE_DIR} 
		${SYNTHESIZER_INCLUDE_DIR} 
		${GUI_INCLUDE_DIR} 
		${RTAUDIO_DIR}
	)
	
	target_link_libraries(gammou_standalone PRIVATE 
		${STANDALONE_APPLICATION_SOUND_SYSTEM_LIBS} 
		view 
		synthesizer
	)
	
	target_compile_definitions(gammou_standalone PRIVATE GAMMOU_VIEW_APP)
endif()

if(GAMMOU_VST3_BUILD)
    set_option(VIEW_VST3 ON)   # Enable VST3 Window in View
	
	set(VST3_SDK_DIR 
		"${CMAKE_SOURCE_DIR}/../VST_SDK/VST3_SDK" 
		CACHE PATH "Choose VST3 SDK path"
	)	

	# Hide VST3 sdk options
	mark_as_advanced(FORCE 
		SMTG_ADD_VST3_HOSTING_SAMPLES
		SMTG_ADD_VST3_PLUGINS_SAMPLES
		SMTG_ADD_VSTGUI
		SMTG_CREATE_VST2_VERSION
		SMTG_CREATE_VST3_LINK
		SMTG_ENABLE_USE_OF_JACK
		SMTG_MYPLUGINS_SRC_PATH
		SMTG_RENAME_ASSERT
		SMTG_USE_STATIC_CRT
		SMTG_VST3_TARGET_PATH
		SMTG_RUN_VST_VALIDATOR
	)
	
	
	add_subdirectory(${VST3_SDK_DIR} VST3_SDK EXCLUDE_FROM_ALL)	# only need base and sdk targets
	
	add_library(gammou MODULE 
		${VST3_SDK_DIR}/public.sdk/source/vst/vstsinglecomponenteffect.cpp
		Application/vst3/factory.cpp
		Application/vst3/vst3_plugin.cpp
		${VST3_PLUGIN_SRC} ${GUI_SRC} ${PERSISTENCE_SRC}
	)
	
	target_include_directories(gammou PRIVATE 
		${VST3_SDK_DIR} 
		${VIEW_INCLUDE_DIR} 
		${SYNTHESIZER_INCLUDE_DIR} 
		${GUI_INCLUDE_DIR}
	)
	
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
    
	# Set Gammou Plugins Directory
	#set(GAMMOU_PLUGINS_DIR 
	#	"**********" 
	#	CACHE PATH "Choose the gammou Plugins directory"
	#)
	
	if(GAMMOU_STANDALONE_BUILD)
		message( FATAL_ERROR "Not implemented" )
	endif()
	
	if(GAMMOU_VST3_BUILD)
		target_sources(gammou PRIVATE "${VST3_SDK_DIR}/public.sdk/source/main/dllmain.cpp")
		set_target_properties(gammou PROPERTIES SUFFIX ".vst3")
		set_target_properties(gammou PROPERTIES LINK_FLAGS "/EXPORT:GetPluginFactory")
		
		#target_compile_definitions(gammou PRIVATE 
		#	GAMMOU_PLUGINS_DIRECTORY_PATH="${GAMMOU_PLUGINS_DIR}"
		#)
		
		# Copy the target in VST3 directory	
		add_post_build_copy(gammou "C:/Program Files/Common Files/VST3")
	endif()

elseif(UNIX AND NOT APPLE)  # LINUX
    
    if(GAMMOU_STANDALONE_BUILD)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
		target_link_libraries(gammou_standalone PRIVATE m jack) # pulse-simple pulse
        target_compile_definitions(gammou_standalone PRIVATE __UNIX_JACK__)#__LINUX_PULSE__)
        target_sources(gammou_standalone PRIVATE Application/standalone/X11/main.cpp)       
    endif()
    
    if(GAMMOU_VST3_BUILD)
        target_sources(gammou PRIVATE "${VST3_SDK_DIR}/public.sdk/source/main/linuxmain.cpp")
    endif()
    
else()
#elseif(APPLE)           # APPLE OSX
    message( FATAL_ERROR "unsupported os" )
endif()


######################################################
#	Sound Gammou Plugins



# Create a Gammou plugin : usage : add_gammou_plugin(plugin_name src1.cpp src2.cpp ....)
macro(add_gammou_plugin name)
	set(plugin_sources ${ARGN})
	add_library(${name} MODULE ${plugin_sources})
	target_link_libraries(${name} PRIVATE synthesizer)
	target_include_directories(${name} PRIVATE ${SYNTHESIZER_INCLUDE_DIR})
	set_target_properties(${name} PROPERTIES 
	    FOLDER "GammouPlugins"
	    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/GammouPlugins"
	    PREFIX ""
	    SUFFIX ".gammou"
	)
endmacro()

add_gammou_plugin(Plugin1 Plugins/plugin1.cpp)
add_gammou_plugin(Plugin2 Plugins/plugin2.cpp)



