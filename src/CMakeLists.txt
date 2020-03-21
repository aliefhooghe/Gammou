cmake_minimum_required (VERSION 3.1)

project(Gammou CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


############################
#                          #
#       Build Options      #
#                          #
############################

option(GAMMOU_ENABLE_DESKTOP_APP "Build a desktop application" ON)

############################
#                          #
#        COMMON SRC        #
#                          #
############################

set(GAMMOU_COMMON_SRC
    ${CMAKE_CURRENT_SOURCE_DIR}/gui/circuit_editor.h
    ${CMAKE_CURRENT_SOURCE_DIR}/gui/circuit_editor.cpp
)

############################
#                          #
#       DESKTOP APP        #
#                          #
############################

set(GAMMOU_DESKTOP_SRC
    ${CMAKE_CURRENT_SOURCE_DIR}/backends/desktop_application/main.cpp
)

if (GAMMOU_ENABLE_DESKTOP_APP)
    add_executable(gammou_desktop_app ${GAMMOU_DESKTOP_SRC} ${GAMMOU_COMMON_SRC})
    target_include_directories(gammou_desktop_app PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
    target_link_libraries(gammou_desktop_app PUBLIC View DSPJIT)
endif()

############################
#                          #
#           LIBS           #
#                          #
############################

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/libs/View/src View)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/libs/DSPJIT/src DSPJIT)
