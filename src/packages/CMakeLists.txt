
include(${CMAKE_CURRENT_SOURCE_DIR}/../cmake/AddGammouPackage.cmake)


#####################
#        Base       #
#####################

add_gammou_plugin(
    Tanh
    ${CMAKE_CURRENT_SOURCE_DIR}/base/tanh_plugin/plugin.json
    ${CMAKE_CURRENT_SOURCE_DIR}/base/tanh_plugin/process.c)

add_gammou_plugin(
    HalfTanh
    ${CMAKE_CURRENT_SOURCE_DIR}/base/half_tanh/plugin.json
    ${CMAKE_CURRENT_SOURCE_DIR}/base/half_tanh/process.c)

add_gammou_plugin(
    Invert
    ${CMAKE_CURRENT_SOURCE_DIR}/base/invert/plugin.json
    ${CMAKE_CURRENT_SOURCE_DIR}/base/invert/process.c)

add_gammou_plugin(
    SinPlugin
    ${CMAKE_CURRENT_SOURCE_DIR}/base/sin_oscillator/plugin.json
    ${CMAKE_CURRENT_SOURCE_DIR}/base/sin_oscillator/process.c)

add_gammou_plugin(
    SawPlugin
    ${CMAKE_CURRENT_SOURCE_DIR}/base/saw_oscillator/plugin.json
    ${CMAKE_CURRENT_SOURCE_DIR}/base/saw_oscillator/process.c)

add_gammou_plugin(
    SquarePlugin
    ${CMAKE_CURRENT_SOURCE_DIR}/base/square_oscillator/plugin.json
    ${CMAKE_CURRENT_SOURCE_DIR}/base/square_oscillator/process.c)

add_gammou_plugin(
    Lp1Plugin
    ${CMAKE_CURRENT_SOURCE_DIR}/base/lp1/plugin.json
    ${CMAKE_CURRENT_SOURCE_DIR}/base/lp1/process.c)

add_gammou_plugin(
    Lp2Plugin
    ${CMAKE_CURRENT_SOURCE_DIR}/base/lp2/plugin.json
    ${CMAKE_CURRENT_SOURCE_DIR}/base/lp2/process.c)

add_gammou_plugin(
    BiquadLpPlugin
    ${CMAKE_CURRENT_SOURCE_DIR}/base/biquad_filter/plugin.json
    ${CMAKE_CURRENT_SOURCE_DIR}/base/biquad_filter/process.c)

add_gammou_plugin(
    DCBlocker
    ${CMAKE_CURRENT_SOURCE_DIR}/base/dc_blocker/plugin.json
    ${CMAKE_CURRENT_SOURCE_DIR}/base/dc_blocker/process.c)

add_gammou_plugin(
    ADSRPlugin
    ${CMAKE_CURRENT_SOURCE_DIR}/base/adsr/plugin.json
    ${CMAKE_CURRENT_SOURCE_DIR}/base/adsr/process.c)

add_gammou_package(
    Base
    CONTENT_FILE
        ${CMAKE_CURRENT_SOURCE_DIR}/base/content.json
    PLUGINS
        Invert
        Tanh
        HalfTanh
        SinPlugin
        SawPlugin
        SquarePlugin
        Lp1Plugin
        Lp2Plugin
        BiquadLpPlugin
        DCBlocker
        ADSRPlugin
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

##################################
#          Waveguides            #
##################################

add_gammou_plugin(
    ClosedEnd
    ${CMAKE_CURRENT_SOURCE_DIR}/waveguides/closed_end/plugin.json
    ${CMAKE_CURRENT_SOURCE_DIR}/waveguides/closed_end/process.c)

add_gammou_plugin(
    OpenEnd
    ${CMAKE_CURRENT_SOURCE_DIR}/waveguides/open_end/plugin.json
    ${CMAKE_CURRENT_SOURCE_DIR}/waveguides/open_end/process.c)

add_gammou_plugin(
    Junction
    ${CMAKE_CURRENT_SOURCE_DIR}/waveguides/junction/plugin.json
    ${CMAKE_CURRENT_SOURCE_DIR}/waveguides/junction/process.c)

add_gammou_plugin(
    ReadPoint
    ${CMAKE_CURRENT_SOURCE_DIR}/waveguides/read_point/plugin.json
    ${CMAKE_CURRENT_SOURCE_DIR}/waveguides/read_point/process.c)

add_gammou_plugin(
    WritePoint
    ${CMAKE_CURRENT_SOURCE_DIR}/waveguides/write_point/plugin.json
    ${CMAKE_CURRENT_SOURCE_DIR}/waveguides/write_point/process.c)

add_gammou_plugin(
    Hammer
    ${CMAKE_CURRENT_SOURCE_DIR}/waveguides/hammer/plugin.json
    ${CMAKE_CURRENT_SOURCE_DIR}/waveguides/hammer/process.c)

add_gammou_plugin(
    WaveGuide
    ${CMAKE_CURRENT_SOURCE_DIR}/waveguides/waveguide/plugin.json
    ${CMAKE_CURRENT_SOURCE_DIR}/waveguides/waveguide/process.c)

add_gammou_plugin(
    Delay
    ${CMAKE_CURRENT_SOURCE_DIR}/waveguides/delay/plugin.json
    ${CMAKE_CURRENT_SOURCE_DIR}/waveguides/delay/process.c)

add_gammou_package(
    Waveguides
    CONTENT_FILE
        ${CMAKE_CURRENT_SOURCE_DIR}/waveguides/content.json
    PLUGINS
        ClosedEnd OpenEnd Junction Hammer ReadPoint WritePoint WaveGuide Delay
    COMMON_LIBS
        ${CMAKE_CURRENT_SOURCE_DIR}/waveguides/common_libs/sample_queue.c)


##################################
#           Samplers             #
##################################

add_gammou_plugin(
    Sampler
    ${CMAKE_CURRENT_SOURCE_DIR}/samplers/sampler/plugin.json
    ${CMAKE_CURRENT_SOURCE_DIR}/samplers/sampler/process.c)

add_gammou_plugin(
    Granular
    ${CMAKE_CURRENT_SOURCE_DIR}/samplers/granular/plugin.json
    ${CMAKE_CURRENT_SOURCE_DIR}/samplers/granular/process.c)

add_gammou_package(
    Samplers
    CONTENT_FILE
        ${CMAKE_CURRENT_SOURCE_DIR}/samplers/content.json
    PLUGINS
        Sampler Granular)