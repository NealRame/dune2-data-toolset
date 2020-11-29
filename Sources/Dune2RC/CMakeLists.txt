project(Dune2RC)

find_program(PROTOC_BIN protoc REQUIRED)

set(DUNE2_RC_PB_PROTOS "rc.proto")
set(DUNE2_RC_PB_BASEDIR "${CMAKE_CURRENT_BINARY_DIR}/protobuf")
set(DUNE2_RC_PB_OUTPUT_DIR "${DUNE2_RC_PB_BASEDIR}/${PROJECT_NAME}")
list(APPEND DUNE2_RC_PB_SOURCES
  "${DUNE2_RC_PB_OUTPUT_DIR}/rc.pb.cc"
  "${DUNE2_RC_PB_OUTPUT_DIR}/rc.pb.h"
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

add_library(${PROJECT_NAME} ${DUNE2_RC_PB_SOURCES})
target_compile_features(${PROJECT_NAME}
  PUBLIC
    cxx_std_17
)
target_include_directories(${PROJECT_NAME}
  PUBLIC
    ${DUNE2_RC_PB_BASEDIR}
)
target_link_libraries(${PROJECT_NAME}
  PUBLIC
    CONAN_PKG::protobuf
)
