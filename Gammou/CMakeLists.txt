cmake_minimum_required (VERSION 3.1)

include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/util.cmake")

#################################################

project (Gammou LANGUAGES CXX)

# Use C++ 17
set(CMAKE_CXX_STANDARD 17)

option(GAMMOU_STANDALONE_BUILD "Build a Standalone Application" OFF)
option(GAMMOU_STANDALONE_LIBOSSIA "Enable ossia oscquery in standalone app" OFF)
option(GAMMOU_VST3_BUILD "Build a VST3 Plugin" OFF)

# Hide View Option
mark_as_advanced(FORCE 
	VIEW_APP
	VIEW_VST
)	

# Enable cmake FOLDER propertie
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Set output build paths
set(VST3_OUTPUT_PATH "${CMAKE_BINARY_DIR}/bin" CACHE PATH "")                      	  # gammou (VST3)
set(GAMMOU_PLUGINS_OUTPUT_PATH "${CMAKE_BINARY_DIR}/GammouPlugins" CACHE PATH "")     # gammou Plugins
set(GAMMOU_PLUGINS_DIR ${GAMMOU_PLUGINS_OUTPUT_PATH} CACHE PATH "")
set(LIBRARY_OUTPUT_PATH "${CMAKE_BINARY_DIR}/lib" CACHE PATH "")                      # View, synthesizer, vstsdk, ....
set(EXECUTABLE_OUTPUT_PATH "${CMAKE_BINARY_DIR}/bin" CACHE PATH "")                   # gammou Standalone

#

set_property(DIRECTORY APPEND PROPERTY COMPILE_DEFINITIONS
    $<$<CONFIG:Debug>:_DEBUG>
    $<$<CONFIG:Release>:NDEBUG>
)


#################################################

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
	Application/gui/plugin_request_dialog.h
	Application/gui/plugin_request_dialog.cpp
	Application/gui/control_component/control_component_ids.h
	Application/gui/control_component/control_sound_component.h
	Application/gui/control_component/control_sound_component.cpp
	Application/gui/control_component/gain_sound_component.h
	Application/gui/control_component/gain_sound_component.cpp
	Application/gui/control_component/integer_control_sound_component.h
	Application/gui/control_component/integer_control_sound_component.cpp
	Application/gui/control_component/integer_gui_component.h
	Application/gui/control_component/integer_gui_component.cpp
	Application/gui/control_component/knob_gui_component.h
	Application/gui/control_component/knob_gui_component.cpp
	Application/gui/control_component/slider_gui_component.h
	Application/gui/control_component/slider_gui_component.cpp
	Application/gui/control_component/value_sound_component.h
	Application/gui/control_component/value_sound_component.cpp
)

set(PERSISTENCE_SRC
    Application/persistence/synthesizer_persistence.h
	Application/persistence/synthesizer_persistence.cpp
)


################################################
#                                              #
#       Gammou Standalone Application          #
#                                              #
################################################


if(GAMMOU_STANDALONE_BUILD)
    set_option(VIEW_APP ON) # Enable Application Window in View
    
	set(
    RTAUDIO_DIR "${CMAKE_SOURCE_DIR}/../3rdparty/rtaudio"
    CACHE PATH ""  
  )
	
  add_executable(gammou_standalone
    	Application/standalone/main.cpp
		${GUI_SRC} 
		${PERSISTENCE_SRC}
		"${RTAUDIO_DIR}/RtAudio.cpp"
	)
	
	target_include_directories(gammou_standalone PRIVATE 
		${GUI_INCLUDE_DIR} 
		${RTAUDIO_DIR}
	)
	
	target_link_libraries(gammou_standalone PRIVATE 
		view 
		synthesizer
	)
	
	target_compile_definitions(gammou_standalone PRIVATE 
	    GAMMOU_VIEW_APP
	    GAMMOU_PLUGINS_DIRECTORY_PATH="${GAMMOU_PLUGINS_DIR}"
	)
	
	# OSSIA binding
	
	if (GAMMOU_STANDALONE_LIBOSSIA)
    	set(LIBOSSIA_DIR
    		"${CMAKE_SOURCE_DIR}/../3rdparty/libossia/" 
			CACHE PATH "Choose libossia path"
    	)
    	
    	add_subdirectory(${LIBOSSIA_DIR} tmp/OSSIA EXCLUDE_FROM_ALL)
    	target_link_libraries(gammou_standalone PRIVATE ossia)
    	target_compile_definitions(gammou_standalone PRIVATE OSSIA_BINDING)
    	target_sources(gammou_standalone PRIVATE
    		Application/standalone/ossia_binding/synthesizer_parameter.h
    		Application/standalone/ossia_binding/synthesizer_parameter.cpp
    		Application/standalone/ossia_binding/synthesizer_protocol.h
    		Application/standalone/ossia_binding/synthesizer_protocol.cpp
    	)
    endif()
	
	# Os specific addition
	
    #if(WIN32)                   # MICROSOFT WINDOWS
    
    if(UNIX)                	 # LINUX and OSX
        
        # For Rtaudio
        if(APPLE)               # Only OSX
            find_library(COREAUDIO_LIB CoreAudio)
            find_library(COREFOUNDATION_LIB CoreFoundation)
            target_link_libraries(gammou_standalone PRIVATE ${COREAUDIO_LIB} ${COREFOUNDATION_LIB} pthread)
            target_compile_definitions(gammou_standalone PRIVATE __MACOSX_CORE__)
        else()                  # Only Linux
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
            target_link_libraries(gammou_standalone PRIVATE m stdc++fs jack) # pulse-simple pulse 
            target_compile_definitions(gammou_standalone PRIVATE __UNIX_JACK__)#    __LINUX_PULSE__
        endif()
        
    else()
        message( FATAL_ERROR "Cant Build gammou standalone on this OS")
    endif()
    
    
	
endif()

################################################
#                                              #
#       Gammou VST3 Plugin                     #
#                                              #
################################################

if(GAMMOU_VST3_BUILD)
    set_option(VIEW_VST3 ON)   # Enable VST3 Window in View
	
	set(VST3_SDK_DIR 
		"${CMAKE_SOURCE_DIR}/../3rdparty/vst3sdk" 
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
	
	add_subdirectory(${VST3_SDK_DIR} tmp/VST3_SDK EXCLUDE_FROM_ALL)	# only need base and sdk targets
	
	add_library(gammou MODULE 
		${VST3_SDK_DIR}/public.sdk/source/vst/vstsinglecomponenteffect.cpp
		Application/vst3/factory.cpp
		Application/vst3/vst3_plugin.cpp
		${VST3_PLUGIN_SRC} 
		${GUI_SRC} 
		${PERSISTENCE_SRC}
	)
	
	target_include_directories(gammou PRIVATE 
		${VST3_SDK_DIR} 
		${GUI_INCLUDE_DIR}
	)
	
    target_link_libraries(gammou PRIVATE view synthesizer base sdk)	# base and sdk from VST3_SDK
	target_compile_definitions(gammou PRIVATE 
	    GAMMOU_VIEW_VST3
	    GAMMOU_PLUGINS_DIRECTORY_PATH="${GAMMOU_PLUGINS_DIR}"
	)
	
	set_target_properties(gammou PROPERTIES 
	    LIBRARY_OUTPUT_DIRECTORY "${VST3_OUTPUT_PATH}"
	    POSITION_INDEPENDENT_CODE ON    
	)
	
	# Os specific addition
	
    if(WIN32)                   # MICROSOFT WINDOWS
        target_sources(gammou PRIVATE "${VST3_SDK_DIR}/public.sdk/source/main/dllmain.cpp")
		set_target_properties(gammou PROPERTIES SUFFIX ".vst3")
		set_target_properties(gammou PROPERTIES LINK_FLAGS "/EXPORT:GetPluginFactory")
		
		# Copy the target in VST3 directory	
		add_post_build_copy(gammou "C:/Program Files/Common Files/VST3")
	
		
		# Vst2 compatibility + export /EXPORT:VSTPluginMain
		#target_sources(gammou PRIVATE
		#	${VST3_SDK_DIR}/public.sdk/source/common/memorystream.cpp
		#	${VST3_SDK_DIR}/public.sdk/source/vst/hosting/eventlist.cpp
		#	${VST3_SDK_DIR}/public.sdk/source/vst/hosting/hostclasses.cpp
		#	${VST3_SDK_DIR}/public.sdk/source/vst/hosting/parameterchanges.cpp
		#	${VST3_SDK_DIR}/public.sdk/source/vst/hosting/processdata.cpp
		#	${VST3_SDK_DIR}/public.sdk/source/vst/vst2wrapper/vst2wrapper.h
		#	${VST3_SDK_DIR}/public.sdk/source/vst/vst2wrapper/vst2wrapper.cpp
		#	${VST3_SDK_DIR}/public.sdk/source/vst2.x/audioeffect.cpp
		#	${VST3_SDK_DIR}/public.sdk/source/vst2.x/audioeffectx.cpp
		#)
		
    elseif(UNIX AND NOT APPLE)  # LINUX		
    
    	# Vst2 compatibility + export /EXPORT:VSTPluginMain
		target_sources(gammou PRIVATE
			${VST3_SDK_DIR}/public.sdk/source/common/memorystream.cpp
			${VST3_SDK_DIR}/public.sdk/source/vst/hosting/eventlist.cpp
			${VST3_SDK_DIR}/public.sdk/source/vst/hosting/hostclasses.cpp
			${VST3_SDK_DIR}/public.sdk/source/vst/hosting/parameterchanges.cpp
			${VST3_SDK_DIR}/public.sdk/source/vst/hosting/processdata.cpp
			${VST3_SDK_DIR}/public.sdk/source/vst/vst2wrapper/vst2wrapper.h
			${VST3_SDK_DIR}/public.sdk/source/vst/vst2wrapper/vst2wrapper.cpp
			${VST3_SDK_DIR}/public.sdk/source/vst2.x/audioeffect.cpp
			${VST3_SDK_DIR}/public.sdk/source/vst2.x/audioeffectx.cpp
		)
    
        target_sources(gammou PRIVATE "${VST3_SDK_DIR}/public.sdk/source/main/linuxmain.cpp")
    elseif(APPLE)               # Apple OSX
        target_sources(gammou PRIVATE "${VST3_SDK_DIR}/public.sdk/source/main/macmain.cpp")
    else()
        message( FATAL_ERROR "Cant Build a VST3 Plugin on this OS")
    endif()
	
endif()


################################################
#                                              #
#       Tests			                       #
#                                              #
################################################


## View Test


set_option(VIEW_APP ON)
add_executable(view_test
	tests/View_test/main.cpp
	tests/View_test/test_window.cpp
	tests/View_test/test_window.h
)
target_link_libraries(view_test view)
target_compile_definitions(view_test PRIVATE GAMMOU_VIEW_APP)


## 



#################################################
#################################################
#################################################
#################################################



# Include Gammou View and Synthesizer libs

mark_as_advanced(FORCE 
	VIEW_APP
	VIEW_VST3
)

# Build Gammou View lib
add_subdirectory(View tmp/View)

# Build Gammou Synthesizer lib
add_subdirectory(Synthesizer tmp/Synthesizer)

# Build Plugins
add_subdirectory(Plugins tmp/Plugins)





###################################################################################################################################################
###################################################################################################################################################
###################################################################################################################################################




