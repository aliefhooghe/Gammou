#------------------------------------------------------------------------------
# From https://github.com/preshing/CairoSample
# Usage: find_package(Cairo [REQUIRED])
#
# Sets variables:
#     Cairo_INCLUDE_DIRS
#     Cairo_LIBS
#     Cairo_DLLS
#------------------------------------------------------------------------------

include(FindPackageHandleStandardArgs)

if(WIN32)
    # Todo a refaire
    # Search for Cairo in extern/cairo-windows-1.15.10
    find_path(Cairo_ROOT "include/cairo.h" PATHS "${CMAKE_CURRENT_LIST_DIR}/../extern/cairo-windows-1.15.10" NO_DEFAULT_PATH)
    if(Cairo_ROOT)
        set(Cairo_INCLUDE_DIRS "${Cairo_ROOT}/include")
        if("${CMAKE_GENERATOR}" MATCHES "Win64")
            set(Cairo_LIBS "${Cairo_ROOT}/lib/x64/cairo.lib")
            set(Cairo_DLLS "${Cairo_ROOT}/lib/x64/cairo.dll")
        else()
            set(Cairo_LIBS "${Cairo_ROOT}/lib/x86/cairo.lib")
            set(Cairo_DLLS "${Cairo_ROOT}/lib/x86/cairo.dll")
        endif()
    endif()

    mark_as_advanced(Cairo_ROOT)
    find_package_handle_standard_args(Cairo DEFAULT_MSG Cairo_INCLUDE_DIRS Cairo_LIBS Cairo_DLLS)
else()
    # On MacOS, should be installed via Macports
    # On Ubuntu, install with: apt-get install libcairo2-dev
    find_path(Cairo_INCLUDE_DIRS cairo.h PATH_SUFFIXES cairo)
    find_library(_Cairo_LIB cairo)
    set(Cairo_LIBS ${_Cairo_LIB})
    
    mark_as_advanced(Cairo_INCLUDE_DIRS _Cairo_LIB)
    find_package_handle_standard_args(Cairo DEFAULT_MSG Cairo_INCLUDE_DIRS Cairo_LIBS)
endif()
