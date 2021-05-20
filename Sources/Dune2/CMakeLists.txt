project(Dune2)

add_library(${PROJECT_NAME} EXCLUDE_FROM_ALL
  bmp.cpp
  bmp.hpp
  bswap.hpp
  io.cpp
  io.hpp
  io_lcw.cpp
  pak.cpp
  pak.hpp
  palette.cpp
  palette.hpp
  surface.hpp
  image.cpp
  image.hpp
  image_load_from_cps.cpp
  image_set.cpp
  image_set_load_from_icn.cpp
  image_set_load_from_json.cpp
  image_set_load_from_shp.cpp
  image_set_to_json.cpp
  image_set.hpp
  icon_set.hpp
  icon_set.cpp
  icon_set_load_from_map.cpp
  icon_set_load_from_json.cpp
  icon_set_to_json.cpp
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
    CONAN_PKG::cppcodec
    CONAN_PKG::fmt
    CONAN_PKG::rapidjson
)

set_property(
  TARGET ${PROJECT_NAME}
  PROPERTY RC_PROTOS ${DUNE2_RC_PB_PROTOS}
)