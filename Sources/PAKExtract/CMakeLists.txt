project(PAKExtract)

nr_case_camel_to_snake("${PROJECT_NAME}" TARGET_OUTPUT_NAME)

add_executable(${PROJECT_NAME}
  main.cpp
)
target_compile_features(${PROJECT_NAME}
  PUBLIC cxx_std_17
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
