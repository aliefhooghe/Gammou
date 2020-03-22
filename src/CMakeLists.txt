cmake_minimum_required (VERSION 3.1)

project(Gammou CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules")

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
#       COMMON LIBS        #
#                          #
############################

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/libs/View/src View)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/libs/DSPJIT/src DSPJIT)

############################
#                          #
#       DESKTOP APP        #
#                          #
############################

if (GAMMOU_ENABLE_DESKTOP_APP)
    find_package(RtAudio REQUIRED)

    set(GAMMOU_DESKTOP_SRC
        ${CMAKE_CURRENT_SOURCE_DIR}/backends/desktop_application/desktop_application.h
        ${CMAKE_CURRENT_SOURCE_DIR}/backends/desktop_application/desktop_application.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/backends/desktop_application/main.cpp
    )

    add_executable(gammou_desktop_app ${GAMMOU_DESKTOP_SRC} ${GAMMOU_COMMON_SRC})
    target_include_directories(gammou_desktop_app PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${RTAUDIO_INCLUDE_DIR})
    target_link_libraries(gammou_desktop_app PUBLIC View DSPJIT ${RTAUDIO_LIBRARIES})
endif()
