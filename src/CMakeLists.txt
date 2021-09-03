cmake_minimum_required (VERSION 3.15)

project(Gammou CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (NOT WIN32)
    list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules")
endif()


############################
#                          #
#       Build Options      #
#                          #
############################

option(GAMMOU_ENABLE_DESKTOP_APP "Build a desktop application" OFF)
option(GAMMOU_ENABLE_VST2_PLUGIN "Build a VST2 plugin" ON)
option(GAMMOU_DESKTOP_APP_BENCHMARK_MODE "Enable benchmark mode in desktop application" OFF)
# Use static runtime library on MSVC
if (WIN32)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()

############################
#                          #
#        COMMON SRC        #
#                          #
############################

set(GAMMOU_COMMON_SRC
    ${CMAKE_CURRENT_SOURCE_DIR}/application/application.h
    ${CMAKE_CURRENT_SOURCE_DIR}/application/application.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/application/patch_browser.h
    ${CMAKE_CURRENT_SOURCE_DIR}/application/patch_browser.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/backends/common/configuration.h
    ${CMAKE_CURRENT_SOURCE_DIR}/backends/common/configuration.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/builtin_plugins/additional_builtin_nodes.h
    ${CMAKE_CURRENT_SOURCE_DIR}/builtin_plugins/additional_builtin_nodes.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/builtin_plugins/load_builtin_plugins.h
    ${CMAKE_CURRENT_SOURCE_DIR}/builtin_plugins/load_builtin_plugins.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/builtin_plugins/node_widget_builtin_plugin.h

    ${CMAKE_CURRENT_SOURCE_DIR}/gui/control_node_widgets/knob_node_widget.h
    ${CMAKE_CURRENT_SOURCE_DIR}/gui/control_node_widgets/knob_node_widget.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/gui/control_node_widgets/load_control_plugins.h
    ${CMAKE_CURRENT_SOURCE_DIR}/gui/control_node_widgets/load_control_plugins.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/gui/control_node_widgets/parameter_serialization.h
    ${CMAKE_CURRENT_SOURCE_DIR}/gui/control_node_widgets/parameter_serialization.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/gui/circuit_editor.h
    ${CMAKE_CURRENT_SOURCE_DIR}/gui/circuit_editor.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/gui/configuration_tree.h
    ${CMAKE_CURRENT_SOURCE_DIR}/gui/configuration_tree.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/gui/composite_node_plugin.h
    ${CMAKE_CURRENT_SOURCE_DIR}/gui/composite_node_plugin.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/gui/configuration_widget.h
    ${CMAKE_CURRENT_SOURCE_DIR}/gui/configuration_widget.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/gui/factory_widget.h
    ${CMAKE_CURRENT_SOURCE_DIR}/gui/factory_widget.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/gui/internal_node_widget.h
    ${CMAKE_CURRENT_SOURCE_DIR}/gui/internal_node_widget.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/gui/synthesizer_gui.h
    ${CMAKE_CURRENT_SOURCE_DIR}/gui/synthesizer_gui.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/plugin_system/configuration/abstract_configuration_page.h
    ${CMAKE_CURRENT_SOURCE_DIR}/plugin_system/configuration/abstract_configuration_directory.h
    ${CMAKE_CURRENT_SOURCE_DIR}/plugin_system/ir_loader.h
    ${CMAKE_CURRENT_SOURCE_DIR}/plugin_system/ir_loader.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/plugin_system/node_widget_external_plugin.h
    ${CMAKE_CURRENT_SOURCE_DIR}/plugin_system/node_widget_external_plugin.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/plugin_system/node_widget_factory.h
    ${CMAKE_CURRENT_SOURCE_DIR}/plugin_system/node_widget_factory.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/plugin_system/package_loader.h
    ${CMAKE_CURRENT_SOURCE_DIR}/plugin_system/package_loader.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/synthesizer/midi_parser.h
    ${CMAKE_CURRENT_SOURCE_DIR}/synthesizer/midi_parser.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/synthesizer/parameter_manager.h
    ${CMAKE_CURRENT_SOURCE_DIR}/synthesizer/parameter_manager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/synthesizer/synthesizer.h
    ${CMAKE_CURRENT_SOURCE_DIR}/synthesizer/synthesizer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/synthesizer/voice_manager.h
    ${CMAKE_CURRENT_SOURCE_DIR}/synthesizer/voice_manager.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/utils/serialization_helpers.h
    ${CMAKE_CURRENT_SOURCE_DIR}/utils/wav_loader.h
    ${CMAKE_CURRENT_SOURCE_DIR}/utils/wav_loader.cpp
)

############################
#                          #
#       COMMON LIBS        #
#                          #
############################

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/libs/View/src View EXCLUDE_FROM_ALL)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/libs/DSPJIT/src DSPJIT EXCLUDE_FROM_ALL)
find_package(nlohmann_json CONFIG REQUIRED)

############################
#                          #
#       DESKTOP APP        #
#                          #
############################

if (GAMMOU_ENABLE_DESKTOP_APP)
    message(STATUS "Build desktop application")
    find_package(RtAudio REQUIRED)
    find_package(RtMidi REQUIRED)

    set(GAMMOU_DESKTOP_SRC
        ${CMAKE_CURRENT_SOURCE_DIR}/backends/desktop_application/desktop_application.h
        ${CMAKE_CURRENT_SOURCE_DIR}/backends/desktop_application/desktop_application.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/backends/desktop_application/main.cpp
    )

    add_executable(gammou_desktop_app ${GAMMOU_DESKTOP_SRC} ${GAMMOU_COMMON_SRC})
    target_include_directories(gammou_desktop_app PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})# ${RTAUDIO_INCLUDE_DIR} ${RTMIDI_INCLUDE_DIR})
    target_link_libraries(gammou_desktop_app PUBLIC View DSPJIT RtMidi::rtmidi RtAudio::rtaudio)

    if (GAMMOU_DESKTOP_APP_BENCHMARK_MODE)
        target_compile_definitions(gammou_desktop_app PUBLIC "GAMMOU_BENCHMARK_MODE")
    endif()

    if(WIN32)
        # Do not create a console application on windows
        set_target_properties(gammou_desktop_app PROPERTIES LINK_FLAGS "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")
    endif()

endif()

############################
#                          #
#       VST2 PLUGIN        #
#                          #
############################

if (GAMMOU_ENABLE_VST2_PLUGIN)
    message(STATUS "Build vst2 plugin")

    set(GAMMOU_VST_PLUGIN_SRC
        ${CMAKE_CURRENT_SOURCE_DIR}/backends/vst2_plugin/vst_compat.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/backends/vst2_plugin/vst2_plugin.h
        ${CMAKE_CURRENT_SOURCE_DIR}/backends/vst2_plugin/vst2_plugin.cpp
    )

    add_library(gammou_vst2_plugin MODULE ${GAMMOU_VST_PLUGIN_SRC} ${GAMMOU_COMMON_SRC})
    target_include_directories(gammou_vst2_plugin PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
    target_link_libraries(gammou_vst2_plugin PRIVATE View DSPJIT nlohmann_json::nlohmann_json nlohmann_json)

    if(WIN32)
        # On windows symbol are not exported by default
        set_target_properties(gammou_vst2_plugin PROPERTIES LINK_FLAGS "/EXPORT:VSTPluginMain")
    endif()

endif()

############################
#                          #
#    GAMMOU PACKAGES       #
#                          #
############################

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/packages)
