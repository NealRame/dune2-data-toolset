project(Dune2)

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
)
set_target_properties(${PROJECT_NAME}
  PROPERTIES CXX_VISIBILITY_PRESET hidden
)
target_compile_features(${PROJECT_NAME}
  PUBLIC
    cxx_std_17
)
target_include_directories(${PROJECT_NAME}
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
    Dune2RC
    CONAN_PKG::fmt
)