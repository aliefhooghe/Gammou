cmake_minimum_required (VERSION 3.1)

#    Need the python interpreter
find_package (Python3 COMPONENTS Interpreter REQUIRED)
include(CMakeParseArguments)

define_property(
    TARGET PROPERTY "GAMMOU_PLUGIN_CONTENT_FILE"
    BRIEF_DOCS "Json file describing a plugin"
    FULL_DOCS "Json file describing a plugin")

define_property(
    TARGET PROPERTY "GAMMOU_PLUGIN_MODULES"
    BRIEF_DOCS "List of bytecode file generated for this plugin"
    FULL_DOCS "List of bytecode file generated for this plugin")

function(add_llvm_bytecode_object)
    # Parse and check arguments
    cmake_parse_arguments(
        ARGS                                # prefix
        ""                                  # bools
        "SOURCE;OUTPUT"                     # monovalued
        "INCLUDE_DIRECTORIES"               # multivalued
        ${ARGN}
    )

    if (NOT ARGS_SOURCE)
        message(FATAL_ERROR "add_llvm_bytecode_object missing SOURCE argument")
    elseif (NOT ARGS_OUTPUT)
        message(FATAL_ERROR "add_llvm_bytecode_object missing OUTPUT argument")
    endif()

    set (CLANG_IR_FLAGS "-emit-llvm" "-O2")

    if (GAMMOU_PLUGIN_COMMON_INCLUDE_DIRECTORIES)
        foreach(INC_DIR ${GAMMOU_PLUGIN_COMMON_INCLUDE_DIRECTORIES})
            list(APPEND CLANG_IR_FLAGS "-I${INC_DIR}")
        endforeach()
    endif()

    if (ARGS_INCLUDE_DIRECTORIES)
        foreach(INC_DIR ${ARGS_INCLUDE_DIRECTORIES})
            list(APPEND CLANG_IR_FLAGS "-I${INC_DIR}")
        endforeach()
    endif()

    add_custom_command(
        OUTPUT
            ${ARGS_OUTPUT}
        COMMAND
            clang
            ${ARGS_SOURCE}
            -c ${CLANG_IR_FLAGS}
            -o ${ARGS_OUTPUT}
        DEPENDS
            ${source_file}
        COMMAND_EXPAND_LISTS
    )
endfunction()

#########################################
#                                       #
#          Add Gammou Plugin            #
#                                       #
#########################################
function(add_gammou_plugin target)
    # Parse and check arguments
    cmake_parse_arguments(
        ARGS                            # prefix
        ""                                # bools
        "PLUGIN_FILE"                    # monovalued
        "SOURCES;INCLUDE_DIRECTORIES"    # multivalued
        ${ARGN}
    )

    if (NOT ARGS_PLUGIN_FILE)
        message(FATAL_ERROR "add_gammou_plugin missing PLUGIN_FILE argument")
    elseif (NOT ARGS_SOURCES)
        message(FATAL_ERROR "add_gammou_plugin missing SOURCES argument")
    endif()

    #    List of generated llvm ir modules
    set(bytecode_modules)
    set(plugin_dir ${CMAKE_CURRENT_BINARY_DIR}/tmp/${target})

    #   Make plugin directory at configuration time
    file(MAKE_DIRECTORY ${plugin_dir})

    #    Compile each source to llvm bytecode
    foreach(source_file ${ARGS_SOURCES})
        get_filename_component(source_name ${source_file} NAME_WE)
        set(output_file ${plugin_dir}/${source_name}.bc)
        add_llvm_bytecode_object(
            SOURCE
                ${source_file}
            OUTPUT
                ${output_file}
            INCLUDE_DIRECTORIES
                ${ARGS_INCLUDE_DIRECTORIES}
        )
        list(APPEND bytecode_modules ${output_file})
    endforeach()

    #    Generate content file
    set(plugin_content_file ${plugin_dir}/content.json)
    add_custom_command(
        OUTPUT
            ${plugin_content_file}
        COMMAND
            Python3::Interpreter
        ARGS
            ${GAMMOU_PLUGIN_GENERATOR_PATH}
            --output ${plugin_content_file}
            --plugin-content ${ARGS_PLUGIN_FILE}
            --modules ${bytecode_modules}
        DEPENDS
            ${ARGS_PLUGIN_FILE}
    )

    #    Create the target
    add_custom_target(
        ${target} ALL
        DEPENDS ${bytecode_modules} ${plugin_content_file}
    )

    set_target_properties(
        ${target} PROPERTIES
        GAMMOU_PLUGIN_CONTENT_FILE ${plugin_content_file}
        GAMMOU_PLUGIN_MODULES "${bytecode_modules}"
    )

endfunction()

#########################################
#                                       #
#         Add Gammou Package            #
#                                       #
#########################################
function(add_gammou_package target)
    # parse and check arguments
    cmake_parse_arguments(
        ARGS                                        # prefix
        ""                                            # bools
        "CONTENT_FILE"                                # monovalued
        "PLUGINS;COMMON_LIBS;INCLUDE_DIRECTORIES"    # multivalued
        ${ARGN}
    )

    if (NOT ARGS_CONTENT_FILE)
        message(FATAL_ERROR "add_gammou_package missing CONTENT_FILE argument")
    endif()

    #    Create package directory
    set(package_dir ${CMAKE_CURRENT_BINARY_DIR}/out/${target})
    file(MAKE_DIRECTORY ${package_dir})

    set(all_plugin_content_files)
    set(package_lib_modules)
    set(package_plugin_modules)

    #    Add each plugin if any
    if (ARGS_PLUGINS)
        foreach(plugin ${ARGS_PLUGINS})
            get_target_property(plugin_content_file ${plugin} GAMMOU_PLUGIN_CONTENT_FILE)
            get_target_property(plugin_modules ${plugin} GAMMOU_PLUGIN_MODULES)

            #    Create plugin directory
            set(plugin_dir ${package_dir}/${plugin})
            file(MAKE_DIRECTORY ${plugin_dir})

            #    Add each module
            foreach(module ${plugin_modules})
                get_filename_component(module_filename ${module} NAME)
                set(output_module ${plugin_dir}/${module_filename})
                add_custom_command(
                    OUTPUT
                        ${output_module}
                    COMMAND
                        ${CMAKE_COMMAND}
                    ARGS
                        -E copy ${module} ${output_module}
                    DEPENDS
                        ${module}
                )
                list(APPEND package_plugin_modules ${output_module})
            endforeach()
            #
            list(APPEND all_plugin_content_files ${plugin_content_file})
        endforeach()
    endif()

    #    add additionals libs if any
    if (ARGS_COMMON_LIBS)
        #    Create package lib directory
        set(package_libs_dir ${package_dir}/lib)
        file(MAKE_DIRECTORY ${package_libs_dir})

        foreach(source_file ${ARGS_COMMON_LIBS})
            get_filename_component(source_name ${source_file} NAME_WE)
            set(output_file ${package_libs_dir}/${source_name}.bc)
            add_llvm_bytecode_object(
                SOURCE
                    ${source_file}
                OUTPUT
                    ${output_file}
                INCLUDE_DIRECTORIES
                    ${ARGS_INCLUDE_DIRECTORIES})
            list(APPEND package_lib_modules ${output_file})
        endforeach()
    endif()

    #    Create package content file
    set(package_content_file ${package_dir}/content.json)
    add_custom_command(
        OUTPUT
            ${package_content_file}
        COMMAND
            Python3::Interpreter
        ARGS
            ${GAMMOU_PACKAGE_GENERATOR_PATH}
            --output ${package_content_file}
            --package-content ${ARGS_CONTENT_FILE}
            --plugins-content ${all_plugin_content_files}
            --common-libs ${package_lib_modules}
        DEPENDS
            ${GAMMOU_PACKAGE_GENERATOR_PATH}
            ${ARGS_CONTENT_FILE}
            ${all_plugin_content_files}
)

    #    Create the target
    add_custom_target(
        ${target}
        ALL
        DEPENDS
            ${package_content_file}
            ${ARGS_PLUGINS}
            ${package_plugin_modules}
            ${package_lib_modules}
    )

endfunction()