project(paltobmp)

add_executable(${PROJECT_NAME}
  main.cpp
)
target_compile_features(${PROJECT_NAME}
  PRIVATE cxx_std_17
)
target_link_libraries(${PROJECT_NAME}
  PRIVATE
    dune2
    CONAN_PKG::boost
)
set_target_properties(${PROJECT_NAME}
  PROPERTIES CXX_VISIBILITY_PRESET hidden
)
install(
  TARGETS ${PROJECT_NAME}
  RUNTIME DESTINATION bin
)
