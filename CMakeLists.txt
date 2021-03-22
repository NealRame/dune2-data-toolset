cmake_minimum_required(VERSION 3.15)

project(Dune2DataToolset)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/Scripts/CMake")

include(Conan)
include(Utils)

conan_env(VERSION 1.34.1)
conan_setup(
  CONANFILE ${CMAKE_SOURCE_DIR}/conanfile.txt
  CMAKE_TARGETS
  KEEP_RPATHS
)

option(ADDRESS_SANITIZER "Enable/Disable address sanitizer" OFF)

set(DUNE2_DATA_ARCHIVE "${CMAKE_SOURCE_DIR}/Dune2Data.zip" CACHE FILEPATH "Path to Dune2 data archive.")

add_subdirectory(Sources)
