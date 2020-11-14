cmake_minimum_required(VERSION 3.15)

project(Dune2DataToolset )

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/Scripts/CMake")

include(Conan)
include(Utils)

conan_env(VERSION 1.30.2)
conan_setup(
  CONANFILE ${CMAKE_SOURCE_DIR}/conanfile.txt
  CMAKE_TARGETS
  KEEP_RPATHS
)

option(ADDRESS_SANITIZER "Enable/Disable address sanitizer" OFF)

add_subdirectory(Sources/Dune2)
add_subdirectory(Sources/PAKExtract)
add_subdirectory(Sources/ICNToBMP)
add_subdirectory(Sources/PALToBMP)
add_subdirectory(Sources/SHPToBMP)
nr_get_all_targets(TARGETS)
foreach(target ${TARGETS})
  get_property(target_type TARGET "${target}" PROPERTY TYPE)

  message(DEBUG "target ${target} type is ${target_type}")

  if("${target_type}" STREQUAL "EXECUTABLE")
    nr_set_target_output_name(${target} PREFIX dune2)
  endif()
endforeach()

