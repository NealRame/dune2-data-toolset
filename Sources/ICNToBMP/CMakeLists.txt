project(icntobmp)

add_executable(${PROJECT_NAME}
  main.cpp
)
target_compile_features(${PROJECT_NAME}
  PUBLIC cxx_std_17
)
target_link_libraries(${PROJECT_NAME}
  PRIVATE
    dune2
    CONAN_PKG::boost
)
set_target_properties(${PROJECT_NAME}
  PROPERTIES CXX_VISIBILITY_PRESET hidden
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
install(
  TARGETS ${PROJECT_NAME}
  RUNTIME DESTINATION bin
)
