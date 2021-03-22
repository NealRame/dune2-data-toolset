project(Dune2)

set(DUNE2_RC_PB_DIR "${CMAKE_CURRENT_BINARY_DIR}/protobuf")
list(APPEND DUNE2_RC_PB_SOURCES
  "${DUNE2_RC_PB_DIR}/resources.pb.cc"
  "${DUNE2_RC_PB_DIR}/resources.pb.h"
)

add_custom_command(
  OUTPUT
    ${DUNE2_RC_PB_SOURCES}
  DEPENDS
    ${DUNE2_RC_PROTO_DIR}/${DUNE2_RC_PROTO}
  COMMENT "Running cpp protocol buffer compiler on ${DUNE2_RC_PROTO}"
  COMMAND ${CMAKE_COMMAND} -E make_directory "${DUNE2_RC_PB_DIR}"
  COMMAND
    "${PROTOC_BIN}"
        --proto_path="${DUNE2_RC_PROTO_DIR}"
        --cpp_out="${DUNE2_RC_PB_DIR}"
      "${DUNE2_RC_PROTO}"
  USES_TERMINAL
)

add_library(${PROJECT_NAME} EXCLUDE_FROM_ALL
  bmp.cpp
  bmp.hpp
  bswap.hpp
  io.cpp
  io.hpp
  pak.cpp
  pak.hpp
  palette.cpp
  palette.hpp
  resources.cpp
  resources.hpp
  resources_json_export.cpp
  surface.hpp
  tileset.cpp
  tileset_load_from_icn.cpp
  tileset_load_from_shp.cpp
  tileset.hpp
  iconset.hpp
  iconset.cpp
  iconset_load_from_map.cpp
  ${DUNE2_RC_PB_SOURCES}
)
target_compile_features(${PROJECT_NAME}
  PUBLIC
    cxx_std_17
)
target_include_directories(${PROJECT_NAME}
  PRIVATE
    ${DUNE2_RC_PB_DIR}
  PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/..
)
if(ADDRESS_SANITIZER)
  target_compile_options(${PROJECT_NAME}
    PUBLIC
      -O0 -fno-omit-frame-pointer -fsanitize=address
  )
  target_link_options(${PROJECT_NAME}
    PUBLIC
      -fno-omit-frame-pointer -fsanitize=address
  )
endif()
target_link_libraries(${PROJECT_NAME}
  PUBLIC
    CONAN_PKG::fmt
    CONAN_PKG::protobuf
    CONAN_PKG::rapidjson
)

set_property(
  TARGET ${PROJECT_NAME}
  PROPERTY RC_PROTOS ${DUNE2_RC_PB_PROTOS}
)