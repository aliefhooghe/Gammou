
if(gammou_util_cmake_included)
	return()
endif()
set(gammou_util_cmake_included true)

# Macro for setting option in subproject
macro(set_option opt value)
  set(${opt} ${value} CACHE INTERNAL "" FORCE)
endmacro()

# Macro to copy target after build
macro(add_post_build_copy target directory)
	file(MAKE_DIRECTORY ${directory})
	add_custom_command(TARGET ${target} POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${target}> ${directory}
		COMMENT "Copying ${target} to ${directory}"
	)
endmacro()