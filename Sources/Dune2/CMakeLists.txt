project(Dune2)

find_program(PROTOC_BIN protoc REQUIRED)

set(DUNE2_RC_PB_PROTOS "dune2_rc.proto")
set(DUNE2_RC_PB_OUTPUT_DIR "${CMAKE_CURRENT_BINARY_DIR}/protobuf")
list(APPEND DUNE2_RC_PB_SOURCES
  "${DUNE2_RC_PB_OUTPUT_DIR}/dune2_rc.pb.cc"
  "${DUNE2_RC_PB_OUTPUT_DIR}/dune2_rc.pb.h"
)
list(APPEND DUNE2_RC_PB_JS
  "${CMAKE_CURRENT_BINARY_DIR}/dune2_rc_pb.js"
)

add_custom_command(
  OUTPUT ${DUNE2_RC_PB_SOURCES}
  DEPENDS ${DUNE2_RC_PB_PROTOS}
  COMMENT "Running cpp protocol buffer compiler on ${PROJECT_NAME}/${DUNE2_RC_PB_PROTOS}"
  COMMAND ${CMAKE_COMMAND} -E make_directory "${DUNE2_RC_PB_OUTPUT_DIR}"
  COMMAND
    "${PROTOC_BIN}"
        --proto_path="${CMAKE_CURRENT_SOURCE_DIR}"
        --cpp_out="${DUNE2_RC_PB_OUTPUT_DIR}"
      "${DUNE2_RC_PB_PROTOS}"
  USES_TERMINAL
)

add_library(${PROJECT_NAME}
  bmp.cpp
  bmp.hpp
  bswap.hpp
  io.cpp
  io.hpp
  pak.cpp
  pak.hpp
  palette.cpp
  palette.hpp
  resource.cpp
  resource.hpp
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
set_target_properties(${PROJECT_NAME}
  PROPERTIES CXX_VISIBILITY_PRESET hidden
)
target_compile_features(${PROJECT_NAME}
  PUBLIC
    cxx_std_17
)
target_include_directories(${PROJECT_NAME}
  PRIVATE
    ${DUNE2_RC_PB_OUTPUT_DIR}
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
)

##############################################################################
## Javascript protocol buffer code generation target

add_custom_target(dune2-rc-js
  ALL
  DEPENDS ${DUNE2_RC_PB_PROTOS}
  BYPRODUCTS ${DUNE2_RC_PB_JS}
  COMMENT "Running javascript protocol buffer compiler on ${PROJECT_NAME}/${DUNE2_RC_PB_PROTOS}"
  COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_CURRENT_BINARY_DIR}"
  COMMAND
    "${PROTOC_BIN}"
        --proto_path="${CMAKE_CURRENT_SOURCE_DIR}"
        --js_out=import_style=commonjs,binary:"${CMAKE_CURRENT_BINARY_DIR}"
      "${DUNE2_RC_PB_PROTOS}"
  USES_TERMINAL
)

install(
  FILES ${DUNE2_RC_PB_JS}
  TYPE LIB
  RENAME dune2-rc.js
)