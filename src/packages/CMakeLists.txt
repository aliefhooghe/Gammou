
include(${CMAKE_CURRENT_SOURCE_DIR}/../cmake/AddGammouPackage.cmake)


#####################
#        Base       #
#####################

add_gammou_plugin(
    Tanh
    ${CMAKE_CURRENT_SOURCE_DIR}/base/tanh_plugin/plugin.json
    ${CMAKE_CURRENT_SOURCE_DIR}/base/tanh_plugin/process.c)

add_gammou_plugin(
    SinPlugin
    ${CMAKE_CURRENT_SOURCE_DIR}/base/sin_oscillator/plugin.json
    ${CMAKE_CURRENT_SOURCE_DIR}/base/sin_oscillator/process.c)

add_gammou_plugin(
    SawPlugin
    ${CMAKE_CURRENT_SOURCE_DIR}/base/saw_oscillator/plugin.json
    ${CMAKE_CURRENT_SOURCE_DIR}/base/saw_oscillator/process.c)

add_gammou_plugin(
    Lp1Plugin
    ${CMAKE_CURRENT_SOURCE_DIR}/base/lp1/plugin.json
    ${CMAKE_CURRENT_SOURCE_DIR}/base/lp1/process.c)

add_gammou_plugin(
    Lp2Plugin
    ${CMAKE_CURRENT_SOURCE_DIR}/base/lp2/plugin.json
    ${CMAKE_CURRENT_SOURCE_DIR}/base/lp2/process.c)

add_gammou_plugin(
    DCBlocker
    ${CMAKE_CURRENT_SOURCE_DIR}/base/dc_blocker/plugin.json
    ${CMAKE_CURRENT_SOURCE_DIR}/base/dc_blocker/process.c)

add_gammou_package(
    Base
    CONTENT_FILE
        ${CMAKE_CURRENT_SOURCE_DIR}/base/content.json
    PLUGINS
        Tanh
        SinPlugin
        SawPlugin
        Lp1Plugin
        Lp2Plugin
        DCBlocker
    COMMON_LIBS
        ${CMAKE_CURRENT_SOURCE_DIR}/base/common_libs/math_utils.c)


##################################
#    Virtual Analog Filters      #
##################################

add_gammou_plugin(
    MoogLadder
    ${CMAKE_CURRENT_SOURCE_DIR}/virtual_analog_filters/moog_ladder/plugin.json
    ${CMAKE_CURRENT_SOURCE_DIR}/virtual_analog_filters/moog_ladder/process.c)

add_gammou_package(
    VAFilters
    CONTENT_FILE
        ${CMAKE_CURRENT_SOURCE_DIR}/virtual_analog_filters/content.json
    PLUGINS
        MoogLadder)
