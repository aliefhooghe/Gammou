#
#   Helper to automate dependency resolution using pkgconfig
#

include_guard(GLOBAL)

find_package(PkgConfig)
include(FindPackageHandleStandardArgs)

function(find_pkgconfig_module)
    # Parse and check arguments
    cmake_parse_arguments(
        ARGS								# prefix
        ""									# bools
        "PACKAGE_NAME;PKGCONF_MODULE;TARGET;INCLUDE_DIR_HINT;LIBRARY_HINT"   # monovalued
        ""				                    # multivalued
        ${ARGN}
    )

	if (NOT ARGS_PKGCONF_MODULE)
		message(FATAL_ERROR "find_pkgconfig_module missing PKGCONF_MODULE argument")
	elseif (NOT ARGS_TARGET)
		message(FATAL_ERROR "find_pkgconfig_module missing TARGET argument")
    elseif (NOT ARGS_PACKAGE_NAME)
		message(FATAL_ERROR "find_pkgconfig_module missing PACKAGE_NAME argument")
	endif()

    pkg_check_modules(PKG_${ARGS_PKGCONF_MODULE} ${ARGS_PKGCONF_MODULE})

    if (ARGS_LIBRARY_HINT)
        find_library(
            ${ARGS_PKGCONF_MODULE}_LIBRARY ${ARGS_LIBRARY_HINT}
            HINTS
                ${PKG_${ARGS_PKGCONF_MODULE}_LIBDIR}
                ${PKG_${ARGS_PKGCONF_MODULE}_LIBRARY_DIRS}
        )

        # Static library
        add_library(${ARGS_TARGET} STATIC IMPORTED)
        set_property(TARGET ${ARGS_TARGET} PROPERTY IMPORTED_LOCATION ${${ARGS_PKGCONF_MODULE}_LIBRARY})
    else ()
        # Header only library
        add_library(${ARGS_TARGET} INTERFACE IMPORTED)
    endif()

    if (ARGS_INCLUDE_DIR_HINT)
        find_path(
            ${ARGS_PKGCONF_MODULE}_INCLUDE_DIR ${ARGS_INCLUDE_DIR_HINT}
            HINTS
                ${PKG_${ARGS_PKGCONF_MODULE}_INCLUDEDIR}
                ${PKG_${ARGS_PKGCONF_MODULE}_INCLUDE_DIRS}
        )
        target_include_directories(${ARGS_TARGET} INTERFACE ${${ARGS_PKGCONF_MODULE}_INCLUDE_DIR})
    endif()

    find_package_handle_standard_args(
        ${ARGS_PACKAGE_NAME}
        REQUIRED_VARS PKG_${ARGS_PKGCONF_MODULE}_VERSION
        VERSION_VAR ${PKG_${ARGS_PKGCONF_MODULE}_VERSION})

endfunction()