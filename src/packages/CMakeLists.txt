
include(${CMAKE_CURRENT_SOURCE_DIR}/../cmake/AddGammouPackage.cmake)

add_gammou_plugin(
    SinPlugin
    ${CMAKE_CURRENT_SOURCE_DIR}/experimental/sin/plugin.json
    ${CMAKE_CURRENT_SOURCE_DIR}/experimental/sin/process.c)

add_gammou_package(
    Experimental
    ${CMAKE_CURRENT_SOURCE_DIR}/experimental/content.json
    SinPlugin)