set(NR_OUTPUT_NAME_PREFIX "" CACHE STRING "Global output name prefix")

# Return the list of all targets added from the current directory.
# Code stolen from: https://stackoverflow.com/a/62311397/2537295
function(nr_get_all_targets out_var)
  set(targets)
  macro(nr_get_all_targets_recursive targets dir)
    get_property(subdirectories DIRECTORY ${dir} PROPERTY SUBDIRECTORIES)
    foreach(subdir ${subdirectories})
      nr_get_all_targets_recursive(${targets} ${subdir})
    endforeach()
    get_property(current_targets DIRECTORY ${dir} PROPERTY BUILDSYSTEM_TARGETS)
    list(APPEND ${targets} ${current_targets})
  endmacro()

  nr_get_all_targets_recursive(targets ${CMAKE_CURRENT_SOURCE_DIR})
  set(${out_var} ${targets} PARENT_SCOPE)
endfunction()

# Force the OUTPUT_NAME property to follow some conventions.
function(nr_set_target_output_name target)
  cmake_parse_arguments("" "" "PREFIX" "" ${ARGN})
  string(TOLOWER "${target}" output_name)
  if(_PREFIX)
    set(output_name "${_PREFIX}-${output_name}")
  elseif(NR_OUTPUT_NAME_PREFIX)
    set(output_name "${NR_OUTPUT_NAME_PREFIX}-${output_name}")
  endif()
  message(STATUS "Target '${target}' output name is '${output_name}'")
  set_target_properties(${target} PROPERTIES OUTPUT_NAME "${output_name}")
endfunction(nr_set_target_output_name)

