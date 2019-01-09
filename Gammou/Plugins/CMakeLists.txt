
cmake_minimum_required (VERSION 3.1)

include("${CMAKE_CURRENT_SOURCE_DIR}/../cmake/util.cmake")

#######################################################

# Guard

if(TARGET GammouPlugins)
	return()
endif()

# Enable cmake FOLDER propertie
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

#######################################################

add_subdirectory(../Synthesizer tmp/Synthesizer)

#######################################################

# set install path

if(WIN32)                   # MICROSOFT WINDOWS
  set(GAMMOU_PLUGINS_INSTALL_PATH "Common Files/Gammou/Plugins" CACHE PATH "")
elseif(UNIX)

  if(APPLE)                 # APPLE OSX
    #TODO
  else()                    # Linux
    set(GAMMOU_PLUGINS_INSTALL_PATH "$ENV{HOME}/.Gammou/Plugins" CACHE PATH "")
  endif()
endif()

#######################################################
project(GammouPlugins LANGUAGES C CXX)	# c is for wav
set(CMAKE_CXX_STANDARD 17)

if(NOT DEFINED GAMMOU_PLUGINS_OUTPUT_PATH)
  set(GAMMOU_PLUGINS_OUTPUT_PATH "${CMAKE_BINARY_DIR}/GammouPlugins" CACHE PATH "")
endif()


# Create a Gammou plugin ; usage : add_gammou_plugin(plugin_name src1.cpp src2.cpp ....)
macro(add_gammou_plugin name)
	set(plugin_sources ${ARGN})
	add_library(${name} MODULE 
		${plugin_sources}
		plugins_ids.h
		plugin_helper.h
	)
	target_link_libraries(${name} PRIVATE synthesizer)
	set_target_properties(
	    ${name} PROPERTIES 
	    FOLDER "GammouPlugins"
	    LIBRARY_OUTPUT_DIRECTORY "${GAMMOU_PLUGINS_OUTPUT_PATH}"  # override LIBRARY_OUTPUT_PATH
	    PREFIX ""
	    SUFFIX ".gammou"
	)
	install(
	    TARGETS ${name}
	    LIBRARY DESTINATION "${GAMMOU_PLUGINS_INSTALL_PATH}"
	)
endmacro()


####################################################################################################################
####################################################################################################################
####################################################################################################################

##	Lib wav

add_library(wav STATIC wav.h wav.c)
set_target_properties(wav PROPERTIES POSITION_INDEPENDENT_CODE ON)

## Lib granular base

add_library(granular_base granular_base.h granular_base.cpp)
set_target_properties(granular_base PROPERTIES POSITION_INDEPENDENT_CODE ON)
target_link_libraries(granular_base PUBLIC synthesizer)

###############################################
###############################################
###############################################

#add_gammou_plugin(Template template.cpp)


add_gammou_plugin(Sampler sampler.cpp)
target_link_libraries(Sampler PRIVATE 	wav)

add_gammou_plugin(Granular granular.cpp)
target_link_libraries(Granular PRIVATE 	wav)
target_link_libraries(Granular PRIVATE 	granular_base)

add_gammou_plugin(RTGranular rt_granular.cpp)
target_link_libraries(RTGranular PRIVATE 	granular_base)

#add_gammou_plugin(OscGranular osc_granular.cpp)
#target_link_libraries(OscGranular PRIVATE granular_base)

add_gammou_plugin(BitCrusher bit_crusher.cpp)
add_gammou_plugin(Noise noise.cpp)
add_gammou_plugin(Delay delay.cpp)
add_gammou_plugin(DCBlocker dc_blocker.cpp)
add_gammou_plugin(LadderFilter ladder_filter.cpp)
add_gammou_plugin(Lp2 lp2.cpp)
#add_gammou_plugin(Lp1 lp1.cpp)
add_gammou_plugin(Notch notch_filter.cpp)


############ 	Joystick Plugin

#find_package(SDL2 REQUIRED)
#add_gammou_plugin(Joystick joystick.cpp)
#target_include_directories(Joystick PRIVATE ${SDL2_INCLUDE_DIRS})
#target_link_libraries(Joystick PRIVATE SDL2)




