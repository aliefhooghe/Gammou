
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
project(GammouPlugins LANGUAGES C CXX)	# c is for wav
set(CMAKE_CXX_STANDARD 17)

if(NOT DEFINED GAMMOU_PLUGINS_OUTPUT_PATH)
set(GAMMOU_PLUGINS_OUTPUT_PATH 
	"${CMAKE_BINARY_DIR}/GammouPlugins"
) # gammou Plugins
endif()

if(NOT DEFINED GAMMOU_PLUGINS_DIR)
set(GAMMOU_PLUGINS_DIR                       
	${GAMMOU_PLUGINS_OUTPUT_PATH}    
	CACHE PATH "Choose the gammou Plugins directory"
)
endif()

message( STATUS "Gammou Plugins Will be Built in ${GAMMOU_PLUGINS_OUTPUT_PATH}")
message( STATUS "Gammou Plugins Will be Copied in ${GAMMOU_PLUGINS_DIR}")

# Create a Gammou plugin ; usage : add_gammou_plugin(plugin_name src1.cpp src2.cpp ....)
macro(add_gammou_plugin name)
	set(plugin_sources ${ARGN})
	add_library(${name} MODULE 
		${plugin_sources}
		plugins_ids.h
		plugin_helper.h)
	target_link_libraries(${name} PRIVATE synthesizer)
	set_target_properties(${name} PROPERTIES 
	    FOLDER "GammouPlugins"
	    LIBRARY_OUTPUT_DIRECTORY ${GAMMOU_PLUGINS_OUTPUT_PATH}
	    PREFIX ""
	    SUFFIX ".gammou"
	)
	
	if(NOT "${GAMMOU_PLUGINS_OUTPUT_PATH}" STREQUAL "${GAMMOU_PLUGINS_DIR}")
	    add_post_build_copy(${name} ${GAMMOU_PLUGINS_DIR})
    endif()
endmacro()


####################################################################################################################
####################################################################################################################
####################################################################################################################

##	Lib wav

add_library(wav STATIC wav.h wav.c)
set_target_properties(wav PROPERTIES POSITION_INDEPENDENT_CODE ON)

###############################################
###############################################
###############################################

add_gammou_plugin(Template template.cpp)
add_gammou_plugin(Plugin1 plugin1.cpp)
add_gammou_plugin(Tanh tanh.cpp)
add_gammou_plugin(BitCrusher bit_crusher.cpp)
add_gammou_plugin(LadderFilter ladder_filter.cpp)
add_gammou_plugin(Test test.cpp)

add_gammou_plugin(Sampler sampler.cpp)
target_link_libraries(Sampler PRIVATE 	wav)

add_gammou_plugin(Granular granular.cpp)
target_link_libraries(Granular PRIVATE 	wav)

############ 	Joystick Plugin

#find_package(SDL2 REQUIRED)
#add_gammou_plugin(Joystick joystick.cpp)
#target_include_directories(Joystick PRIVATE ${SDL2_INCLUDE_DIRS})
#target_link_libraries(Joystick PRIVATE SDL2)




