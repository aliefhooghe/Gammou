#------------------------------------------------------------------------------
# Inspired By https://github.com/preshing/CairoSample
# Usage: find_package(Cairo [REQUIRED])
#
# Sets variables:
#     Cairo_INCLUDE_DIRS
#     Cairo_LIBS
#------------------------------------------------------------------------------

include(FindPackageHandleStandardArgs)

if(WIN32)
    # Todo a refaire
    # Ultra degeu
	
	set(Cairo_DIR "D:/Gammou/Cairo")
    set(Cairo_INCLUDE_DIRS "${Cairo_DIR}/include")
	set(Cairo_LIB_DIR "${Cairo_DIR}/lib")
	
    if("${CMAKE_GENERATOR}" MATCHES "Win64")
		
		set(Cairo_LIBS 
			"${Cairo_LIB_DIR}/x64/${CMAKE_BUILD_TYPE}/cairo.lib" 
			"${Cairo_LIB_DIR}/x64/${CMAKE_BUILD_TYPE}/pixman.lib"
			"${Cairo_LIB_DIR}/x64/${CMAKE_BUILD_TYPE}/freetype.lib"
		)
       
    else()
		 message(FATAL_ERROR "Unimplemented")
    endif()

    find_package_handle_standard_args(Cairo DEFAULT_MSG Cairo_INCLUDE_DIRS Cairo_LIBS)
else()
    # On MacOS, should be installed via Macports
    # On Ubuntu, install with: apt-get install libcairo2-dev
    find_path(Cairo_INCLUDE_DIRS cairo.h PATH_SUFFIXES cairo)
    find_library(_Cairo_LIB cairo)
    set(Cairo_LIBS ${_Cairo_LIB})
    
    mark_as_advanced(Cairo_INCLUDE_DIRS _Cairo_LIB)
    find_package_handle_standard_args(Cairo DEFAULT_MSG Cairo_INCLUDE_DIRS Cairo_LIBS)
endif()
