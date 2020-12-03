project(RCToolkit)

nr_case_camel_to_snake("${PROJECT_NAME}" TARGET_OUTPUT_NAME)

add_executable(${PROJECT_NAME}
  app.cpp
  app.hpp
  main.cpp
  commands/palette.cpp
  commands/soundset.cpp
  commands/tileset.cpp
)
target_compile_features(${PROJECT_NAME}
  PRIVATE cxx_std_17
)
target_include_directories(${PROJECT_NAME}
  PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)
target_link_libraries(${PROJECT_NAME}
  PRIVATE
    Dune2
    CONAN_PKG::cli11
    CONAN_PKG::fmt
)
set_target_properties(${PROJECT_NAME}
  PROPERTIES
    CXX_VISIBILITY_PRESET hidden
    OUTPUT_NAME "dune2-${TARGET_OUTPUT_NAME}"
)
install(
  TARGETS ${PROJECT_NAME}
  RUNTIME DESTINATION bin
)
