cmake_minimum_required(VERSION 3.15)

project(dune2-data-toolset)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/Scripts/CMake")

include(Conan)

conan_env(VERSION 1.29.2)
conan_setup(
  CONANFILE ${CMAKE_SOURCE_DIR}/conanfile.txt
  CMAKE_TARGETS
  KEEP_RPATHS
)

add_subdirectory(Sources/Dune2)
add_subdirectory(Sources/PAKExtractor)
add_subdirectory(Sources/PALToBMP)
