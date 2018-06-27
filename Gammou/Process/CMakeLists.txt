
cmake_minimum_required (VERSION 3.1)
#######################################################

# Guard

if(TARGET process)
	return()
endif()

#######################################################
project(Process LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
 
set(PROCESS_SRC
	${CMAKE_CURRENT_SOURCE_DIR}/abstract_component.h
	${CMAKE_CURRENT_SOURCE_DIR}/abstract_frame.h
	${CMAKE_CURRENT_SOURCE_DIR}/circuit_frame.h
	${CMAKE_CURRENT_SOURCE_DIR}/frame_component.h
	${CMAKE_CURRENT_SOURCE_DIR}/gammou_process.h
	${CMAKE_CURRENT_SOURCE_DIR}/observer.h
	${CMAKE_CURRENT_SOURCE_DIR}/helper_components/buffer_fetcher.h
	${CMAKE_CURRENT_SOURCE_DIR}/helper_components/buffer_filler.h
	${CMAKE_CURRENT_SOURCE_DIR}/helper_components/buffer_sum.h
)

add_library(process INTERFACE)
target_sources(process INTERFACE ${PROCESS_SRC})
target_include_directories(process INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

