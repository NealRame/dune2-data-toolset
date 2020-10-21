project(dune2)

add_library(${PROJECT_NAME}
  bmp.cpp
  bmp.hpp
  bswap.hpp
  color.hpp
  io.cpp
  io.hpp
  pak.cpp
  pak.hpp
  palette.cpp
  palette.hpp
  shp.cpp
  shp.hpp
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
