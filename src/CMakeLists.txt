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
option(GAMMOU_ENABLE_VST2_PLUGIN "Build a VST2 plugin" ON)

############################
#                          #
#        COMMON SRC        #
#                          #
############################

set(GAMMOU_COMMON_SRC
    ${CMAKE_CURRENT_SOURCE_DIR}/backends/common/configuration.h
    ${CMAKE_CURRENT_SOURCE_DIR}/backends/common/configuration.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/builtin_plugins/load_builtin_plugins.h
    ${CMAKE_CURRENT_SOURCE_DIR}/builtin_plugins/load_builtin_plugins.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/builtin_plugins/node_widget_builtin_plugin.h

    ${CMAKE_CURRENT_SOURCE_DIR}/gui/circuit_editor.h
    ${CMAKE_CURRENT_SOURCE_DIR}/gui/circuit_editor.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/gui/main_gui.h
    ${CMAKE_CURRENT_SOURCE_DIR}/gui/main_gui.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/plugin_system/node_widget_external_plugin.h
    ${CMAKE_CURRENT_SOURCE_DIR}/plugin_system/node_widget_external_plugin.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/plugin_system/node_widget_factory.h
    ${CMAKE_CURRENT_SOURCE_DIR}/plugin_system/node_widget_factory.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/plugin_system/package_loader.h
    ${CMAKE_CURRENT_SOURCE_DIR}/plugin_system/package_loader.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/synthesizer/midi_parser.h
    ${CMAKE_CURRENT_SOURCE_DIR}/synthesizer/midi_parser.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/synthesizer/parameter_manager.h
    ${CMAKE_CURRENT_SOURCE_DIR}/synthesizer/synthesizer.h
    ${CMAKE_CURRENT_SOURCE_DIR}/synthesizer/synthesizer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/synthesizer/voice_manager.h
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
    find_package(RtMidi REQUIRED)

    set(GAMMOU_DESKTOP_SRC
        ${CMAKE_CURRENT_SOURCE_DIR}/backends/desktop_application/desktop_application.h
        ${CMAKE_CURRENT_SOURCE_DIR}/backends/desktop_application/desktop_application.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/backends/desktop_application/main.cpp
    )

    add_executable(gammou_desktop_app ${GAMMOU_DESKTOP_SRC} ${GAMMOU_COMMON_SRC})
    target_include_directories(gammou_desktop_app PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${RTAUDIO_INCLUDE_DIR} ${RTMIDI_INCLUDE_DIR})
    target_link_libraries(gammou_desktop_app PUBLIC View DSPJIT ${RTMIDI_LIBRARIES} ${RTAUDIO_LIBRARIES})
endif()

############################
#                          #
#       VST2 PLUGIN        #
#                          #
############################

if (GAMMOU_ENABLE_VST2_PLUGIN)

    set(GAMMOU_VST_PLUGIN_SRC
        ${CMAKE_CURRENT_SOURCE_DIR}/backends/vst2_plugin/vst_compat.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/backends/vst2_plugin/vst2_plugin.h
        ${CMAKE_CURRENT_SOURCE_DIR}/backends/vst2_plugin/vst2_plugin.cpp
    )

    add_library(gammou_vst2_plugin MODULE ${GAMMOU_VST_PLUGIN_SRC} ${GAMMOU_COMMON_SRC})
    target_include_directories(gammou_vst2_plugin PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
    target_link_libraries(gammou_vst2_plugin PUBLIC View DSPJIT)

endif()

############################
#                          #
#    GAMMOU PACKAGES       #
#                          #
############################

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/packages)
