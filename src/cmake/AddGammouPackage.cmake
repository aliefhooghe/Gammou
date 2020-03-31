cmake_minimum_required (VERSION 3.1)

#	Need the python interpreter
find_package (Python3 COMPONENTS Interpreter REQUIRED)

set(PACKAGE_GENERATOR_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../../tools/package_generator.py)
set(PLUGIN_GENERATOR_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../../tools/plugin_generator.py)

define_property(TARGET PROPERTY "GAMMOU_PLUGIN_CONTENT_FILE" BRIEF_DOCS "Json file describing a plugin" FULL_DOCS "fgj")
define_property(TARGET PROPERTY "GAMMOU_PLUGIN_MODULES" BRIEF_DOCS "List of bytecode file generated for this plugin" FULL_DOCS "sghfg")

#########################################
#                                       #
#          Add Gammou Plugin            #
#                                       #
#########################################
function(add_gammou_plugin target plugin_file) # ... sources

	#	List of generated llvm ir modules
	set(bytecode_modules)
	set(plugin_dir ${CMAKE_CURRENT_BINARY_DIR}/${target})

	#   Make plugin directory at configuration time
	file(MAKE_DIRECTORY ${plugin_dir})

	#	Compile each source to llvm bytecode
	foreach(source_file ${ARGN})
		get_filename_component(source_name ${source_file} NAME_WE)
		set(output_name ${plugin_dir}/${source_name}.bc)

		add_custom_command(
			OUTPUT ${output_name}
			COMMAND clang
			ARGS ${source_file} -c -emit-llvm -o ${output_name} ${CMAKE_CXX_FLAGS}
			DEPENDS ${source_file}
		)

		list(APPEND bytecode_modules ${output_name})
	endforeach()

	#	Generate content file
	set(plugin_content_file ${plugin_dir}/content.json)
	add_custom_command(
		OUTPUT ${plugin_content_file}
		COMMAND Python3::Interpreter
		ARGS ${PLUGIN_GENERATOR_PATH} ${plugin_content_file} ${plugin_file} ${bytecode_modules}
		DEPENDS ${plugin_file}
	)

	#	Create the target
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
function(add_gammou_package target package_file) # ... plugins
	#	Create package directory
	set(package_dir ${CMAKE_CURRENT_BINARY_DIR}/${target})
	file(MAKE_DIRECTORY ${package_dir})

	set(all_plugin_content_files)
	set(all_plugin_modules)

	#	Add each plugin
	foreach(plugin ${ARGN})
		get_target_property(plugin_content_file ${plugin} GAMMOU_PLUGIN_CONTENT_FILE)
		get_target_property(plugin_modules ${plugin} GAMMOU_PLUGIN_MODULES)

		#	Create plugin directory
		set(plugin_dir ${package_dir}/${plugin})
		file(MAKE_DIRECTORY ${plugin_dir})

		#	Add each module
		foreach(module ${plugin_modules})
			get_filename_component(module_filename ${module} NAME)
			set(output_module ${plugin_dir}/${module_filename})
			add_custom_command(
				OUTPUT ${output_module}
				COMMAND ${CMAKE_COMMAND}
				ARGS -E copy ${module} ${output_module}
				DEPENDS ${module})
			list(APPEND all_plugin_modules ${output_module})
		endforeach()
		#
		list(APPEND all_plugin_content_files ${plugin_content_file})
	endforeach()

	#	Create package content file
	set(package_content_file ${package_dir}/content.json)
	add_custom_command(
		OUTPUT ${package_content_file}
		COMMAND Python3::Interpreter
		ARGS ${PACKAGE_GENERATOR_PATH} ${package_content_file} ${package_file} ${all_plugin_content_files}
		DEPENDS ${all_plugin_content_files} ${package_file}
	)

	#	Create the target
	add_custom_target(
		${target} ALL
		DEPENDS ${package_content_file} ${all_plugin_modules}
	)

endfunction()