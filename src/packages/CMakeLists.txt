
include(${CMAKE_CURRENT_SOURCE_DIR}/../cmake/AddGammouPackage.cmake)

#####################
#     Oscillators   #
#####################

add_gammou_plugin(
    SinPlugin
    ${CMAKE_CURRENT_SOURCE_DIR}/oscillators/sin/plugin.json
    ${CMAKE_CURRENT_SOURCE_DIR}/oscillators/sin/process.c)

add_gammou_plugin(
    SawPlugin
    ${CMAKE_CURRENT_SOURCE_DIR}/oscillators/saw/plugin.json
    ${CMAKE_CURRENT_SOURCE_DIR}/oscillators/saw/process.c)

add_gammou_package(
    Oscillators
    ${CMAKE_CURRENT_SOURCE_DIR}/oscillators/content.json
    SinPlugin SawPlugin)

#####################
#      Filters      #
#####################

add_gammou_plugin(
    Lp1Plugin
    ${CMAKE_CURRENT_SOURCE_DIR}/filters/lp1/plugin.json
    ${CMAKE_CURRENT_SOURCE_DIR}/filters/lp1/process.c)

add_gammou_plugin(
    MoogLadder
    ${CMAKE_CURRENT_SOURCE_DIR}/filters/moog_ladder/plugin.json
    ${CMAKE_CURRENT_SOURCE_DIR}/filters/moog_ladder/process.c)

add_gammou_package(
    Filters
    ${CMAKE_CURRENT_SOURCE_DIR}/filters/content.json
    Lp1Plugin MoogLadder)