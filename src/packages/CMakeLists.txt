cmake_minimum_required (VERSION 3.15)
project(GammouPackages)

# Include and configure the package generation helpers
include(${CMAKE_CURRENT_SOURCE_DIR}/toolchain/AddGammouPackage.cmake)
set(GAMMOU_PLUGIN_COMMON_INCLUDE_DIRECTORIES ${CMAKE_CURRENT_SOURCE_DIR}/common)
set(GAMMOU_PACKAGE_GENERATOR_PATH ${CMAKE_CURRENT_SOURCE_DIR}/toolchain/package_generator.py)
set(GAMMOU_PLUGIN_GENERATOR_PATH ${CMAKE_CURRENT_SOURCE_DIR}/toolchain/plugin_generator.py)

#####################
#        Base       #
#####################

add_gammou_plugin(
    Absolute
    PLUGIN_FILE
        ${CMAKE_CURRENT_SOURCE_DIR}/base/absolute/plugin.json
    SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/base/absolute/process.c
    INCLUDE_DIRECTORIES
        ${CMAKE_CURRENT_SOURCE_DIR}/base/common_libs)

add_gammou_plugin(
    Clamp
    PLUGIN_FILE
        ${CMAKE_CURRENT_SOURCE_DIR}/base/clamp/plugin.json
    SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/base/clamp/process.c
    INCLUDE_DIRECTORIES
        ${CMAKE_CURRENT_SOURCE_DIR}/base/common_libs)

add_gammou_plugin(
    Exp
    PLUGIN_FILE
        ${CMAKE_CURRENT_SOURCE_DIR}/base/exp/plugin.json
    SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/base/exp/process.c
    INCLUDE_DIRECTORIES
        ${CMAKE_CURRENT_SOURCE_DIR}/base/common_libs)

add_gammou_plugin(
    Noise
    PLUGIN_FILE
        ${CMAKE_CURRENT_SOURCE_DIR}/base/noise/plugin.json
    SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/base/noise/process.c
    INCLUDE_DIRECTORIES
        ${CMAKE_CURRENT_SOURCE_DIR}/base/common_libs)

add_gammou_plugin(
    Tanh
    PLUGIN_FILE
        ${CMAKE_CURRENT_SOURCE_DIR}/base/tanh_plugin/plugin.json
    SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/base/tanh_plugin/process.c
    INCLUDE_DIRECTORIES
        ${CMAKE_CURRENT_SOURCE_DIR}/base/common_libs)

add_gammou_plugin(
    HalfTanh
    PLUGIN_FILE
        ${CMAKE_CURRENT_SOURCE_DIR}/base/half_tanh/plugin.json
    SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/base/half_tanh/process.c
    INCLUDE_DIRECTORIES
        ${CMAKE_CURRENT_SOURCE_DIR}/base/common_libs)

add_gammou_plugin(
    Invert
    PLUGIN_FILE
        ${CMAKE_CURRENT_SOURCE_DIR}/base/invert/plugin.json
    SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/base/invert/process.c
    INCLUDE_DIRECTORIES
        ${CMAKE_CURRENT_SOURCE_DIR}/base/common_libs)

add_gammou_plugin(
    SinPlugin
    PLUGIN_FILE
        ${CMAKE_CURRENT_SOURCE_DIR}/base/sin_oscillator/plugin.json
    SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/base/sin_oscillator/process.c
    INCLUDE_DIRECTORIES
        ${CMAKE_CURRENT_SOURCE_DIR}/base/common_libs)

add_gammou_plugin(
    SawPlugin
    PLUGIN_FILE
        ${CMAKE_CURRENT_SOURCE_DIR}/base/saw_oscillator/plugin.json
    SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/base/saw_oscillator/process.c
    INCLUDE_DIRECTORIES
        ${CMAKE_CURRENT_SOURCE_DIR}/base/common_libs)

add_gammou_plugin(
    SquarePlugin
    PLUGIN_FILE
        ${CMAKE_CURRENT_SOURCE_DIR}/base/square_oscillator/plugin.json
    SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/base/square_oscillator/process.c
    INCLUDE_DIRECTORIES
        ${CMAKE_CURRENT_SOURCE_DIR}/base/common_libs)

add_gammou_plugin(
    Lp1Plugin
    PLUGIN_FILE
        ${CMAKE_CURRENT_SOURCE_DIR}/base/lp1/plugin.json
    SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/base/lp1/process.c
    INCLUDE_DIRECTORIES
        ${CMAKE_CURRENT_SOURCE_DIR}/base/common_libs)

add_gammou_plugin(
    SofterPlugin
    PLUGIN_FILE
        ${CMAKE_CURRENT_SOURCE_DIR}/base/softer/plugin.json
    SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/base/softer/process.c
    INCLUDE_DIRECTORIES
        ${CMAKE_CURRENT_SOURCE_DIR}/base/common_libs)

add_gammou_plugin(
    Lp2Plugin
    PLUGIN_FILE
        ${CMAKE_CURRENT_SOURCE_DIR}/base/lp2/plugin.json
    SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/base/lp2/process.c
    INCLUDE_DIRECTORIES
        ${CMAKE_CURRENT_SOURCE_DIR}/base/common_libs)

add_gammou_plugin(
    Hp2Plugin
    PLUGIN_FILE
        ${CMAKE_CURRENT_SOURCE_DIR}/base/hp2/plugin.json
    SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/base/hp2/process.c
    INCLUDE_DIRECTORIES
        ${CMAKE_CURRENT_SOURCE_DIR}/base/common_libs)

add_gammou_plugin(
    BiquadLpPlugin
    PLUGIN_FILE
        ${CMAKE_CURRENT_SOURCE_DIR}/base/biquad_filter/plugin.json
    SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/base/biquad_filter/process.c
    INCLUDE_DIRECTORIES
        ${CMAKE_CURRENT_SOURCE_DIR}/base/common_libs)

add_gammou_plugin(
    DCBlocker
    PLUGIN_FILE
        ${CMAKE_CURRENT_SOURCE_DIR}/base/dc_blocker/plugin.json
    SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/base/dc_blocker/process.c
    INCLUDE_DIRECTORIES
        ${CMAKE_CURRENT_SOURCE_DIR}/base/common_libs)

add_gammou_plugin(
    ADSRPlugin
    PLUGIN_FILE
        ${CMAKE_CURRENT_SOURCE_DIR}/base/adsr/plugin.json
    SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/base/adsr/process.c
    INCLUDE_DIRECTORIES
        ${CMAKE_CURRENT_SOURCE_DIR}/base/common_libs)

add_gammou_plugin(
    Ramp
    PLUGIN_FILE
        ${CMAKE_CURRENT_SOURCE_DIR}/base/ramp/plugin.json
    SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/base/ramp/process.c
    INCLUDE_DIRECTORIES
        ${CMAKE_CURRENT_SOURCE_DIR}/base/common_libs)

add_gammou_plugin(
    ShannonFreq
    PLUGIN_FILE
        ${CMAKE_CURRENT_SOURCE_DIR}/base/shannon_freq/plugin.json
    SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/base/shannon_freq/process.c)

add_gammou_package(
    Base
    CONTENT_FILE
        ${CMAKE_CURRENT_SOURCE_DIR}/base/content.json
    PLUGINS
        Absolute
        ADSRPlugin
        BiquadLpPlugin
        Clamp
        DCBlocker
        Exp
        Noise
        HalfTanh
        Invert
        Lp1Plugin
        Lp2Plugin
        Hp2Plugin
        Ramp
        SawPlugin
        ShannonFreq
        SinPlugin
        SofterPlugin
        SquarePlugin
        Tanh
    COMMON_LIBS
        ${CMAKE_CURRENT_SOURCE_DIR}/base/common_libs/math_utils.c
    INCLUDE_DIRECTORIES
        ${CMAKE_CURRENT_SOURCE_DIR}/base/common_libs)


##################################
#    Virtual Analog Filters      #
##################################

add_gammou_plugin(
    MoogLadder
    PLUGIN_FILE
        ${CMAKE_CURRENT_SOURCE_DIR}/virtual_analog_filters/moog_ladder/plugin.json
    SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/virtual_analog_filters/moog_ladder/process.c
    INCLUDE_DIRECTORIES
        ${CMAKE_CURRENT_SOURCE_DIR}/base/common_libs)

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
    PLUGIN_FILE
        ${CMAKE_CURRENT_SOURCE_DIR}/waveguides/closed_end/plugin.json
    SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/waveguides/closed_end/process.c)

add_gammou_plugin(
    OpenEnd
    PLUGIN_FILE
        ${CMAKE_CURRENT_SOURCE_DIR}/waveguides/open_end/plugin.json
    SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/waveguides/open_end/process.c)

add_gammou_plugin(
    Junction
    PLUGIN_FILE
        ${CMAKE_CURRENT_SOURCE_DIR}/waveguides/junction/plugin.json
    SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/waveguides/junction/process.c)

add_gammou_plugin(
    ReadPoint
    PLUGIN_FILE
        ${CMAKE_CURRENT_SOURCE_DIR}/waveguides/read_point/plugin.json
    SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/waveguides/read_point/process.c)

add_gammou_plugin(
    WritePoint
    PLUGIN_FILE
        ${CMAKE_CURRENT_SOURCE_DIR}/waveguides/write_point/plugin.json
    SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/waveguides/write_point/process.c)

add_gammou_plugin(
    Hammer
    PLUGIN_FILE
        ${CMAKE_CURRENT_SOURCE_DIR}/waveguides/hammer/plugin.json
    SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/waveguides/hammer/process.c)

add_gammou_plugin(
    WaveGuide
    PLUGIN_FILE
        ${CMAKE_CURRENT_SOURCE_DIR}/waveguides/waveguide/plugin.json
    SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/waveguides/waveguide/process.c)

add_gammou_plugin(
    Delay
    PLUGIN_FILE
        ${CMAKE_CURRENT_SOURCE_DIR}/waveguides/delay/plugin.json
    SOURCES
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
    PLUGIN_FILE
        ${CMAKE_CURRENT_SOURCE_DIR}/samplers/sampler/plugin.json
    SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/samplers/sampler/process.c
    INCLUDE_DIRECTORIES
        ${CMAKE_CURRENT_SOURCE_DIR}/samplers/common_libs
        ${CMAKE_CURRENT_SOURCE_DIR}/base/common_libs)

add_gammou_plugin(
    Granular
    PLUGIN_FILE
        ${CMAKE_CURRENT_SOURCE_DIR}/samplers/granular/plugin.json
    SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/samplers/granular/process.c
    INCLUDE_DIRECTORIES
        ${CMAKE_CURRENT_SOURCE_DIR}/samplers/common_libs
        ${CMAKE_CURRENT_SOURCE_DIR}/base/common_libs)

add_gammou_package(
    Samplers
    CONTENT_FILE
        ${CMAKE_CURRENT_SOURCE_DIR}/samplers/content.json
    PLUGINS
        Sampler Granular
    COMMON_LIBS
        ${CMAKE_CURRENT_SOURCE_DIR}/samplers/common_libs/sample_utils.c)
