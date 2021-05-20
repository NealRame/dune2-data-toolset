set(DUNE2_RC "dune2.rc")

add_subdirectory(PAKExtract)
add_subdirectory(Dune2)
add_subdirectory(RCToolkit)

project(Dune2Data)

include(Dune2DataFiles.cmake)

if(NOT EXISTS ${DUNE2_DATA_ARCHIVE})
  message(FATAL_ERROR "'${DUNE2_DATA_ARCHIVE}' does not exists!")
endif()

# Extract data from Dune2 archive
add_custom_command(
  OUTPUT
    ${DUNE2_PAK_FILES}
  DEPENDS
    ${DUNE2_DATA_ARCHIVE}
  COMMENT
    "Extracting Dune2 data archive"
  COMMAND
    ${CMAKE_COMMAND} -E tar x "${DUNE2_DATA_ARCHIVE}" -- ${DUNE2_PAK_FILES}
)

# Extract Atreides VOC files
add_custom_command(
  OUTPUT ${DUNE2_ATREIDES_VOC_FILES}
  DEPENDS
    PAKExtract
    ${DUNE2_ATRE_PAK_FILES}
  COMMENT
    "Extracting Atreides VOC files"
  COMMAND
    $<TARGET_FILE:PAKExtract> -d ATRE ${DUNE2_ATRE_PAK_FILES}
)

# Extract Harkonnen VOC files
add_custom_command(
  OUTPUT ${DUNE2_HARKONNEN_VOC_FILES}
  DEPENDS
    PAKExtract
    ${DUNE2_HARK_PAK_FILES}
  COMMENT
    "Extracting Harkonnen VOC files"
  COMMAND
    $<TARGET_FILE:PAKExtract> -d HARK ${DUNE2_HARK_PAK_FILES}
)

# Extract Ordos VOC files
add_custom_command(
  OUTPUT ${DUNE2_ORDOS_VOC_FILES}
  DEPENDS
    PAKExtract
    ${DUNE2_ORDOS_PAK_FILES}
  COMMENT
    "Extracting Ordos VOC files"
  COMMAND
    $<TARGET_FILE:PAKExtract> -d ORDOS ${DUNE2_ORDOS_PAK_FILES}
)

# Extract Game fx VOC files
add_custom_command(
  OUTPUT ${DUNE2_GAME_FX_VOC_FILES}
  DEPENDS
    PAKExtract
    ${DUNE2_VOC_PAK_FILES}
  COMMENT
    "Extracting sound fx VOC files"
  COMMAND
    $<TARGET_FILE:PAKExtract> -d FX ${DUNE2_VOC_PAK_FILES}
)

# Extract GPX file
add_custom_command(
  OUTPUT
    ${DUNE2_TILES_FILES}
  DEPENDS
    PAKExtract
    ${DUNE2_DUNE_PAK_FILES}
  COMMENT
    "Extracting images files"
  COMMAND
    $<TARGET_FILE:PAKExtract> -d GFX ${DUNE2_DUNE_PAK_FILES}
)

# Palette
add_custom_command(
  OUTPUT "palette.json"
  DEPENDS
    ${DUNE2_PAL_FILE}
    RCToolkit
  COMMENT
    "Create palette"
  COMMAND
    $<TARGET_FILE:RCToolkit>
      palette create ${DUNE2_PAL_FILE}
)

# Units tileset
add_custom_command(
  OUTPUT "units-images.json"
  DEPENDS
    ${DUNE2_UNITS_FILES}
    RCToolkit
  COMMENT
    "Importing Units tiles"
  COMMAND
    $<TARGET_FILE:RCToolkit>
      images create -o units-images.json ${DUNE2_UNITS_FILES}
)

# Terrain tileset
add_custom_command(
  OUTPUT "terrain-images.json"
  DEPENDS
    ${DUNE2_TERRAIN_FILES}
    RCToolkit
  COMMENT
    "Importing Terrain tiles"
  COMMAND
    $<TARGET_FILE:RCToolkit>
      images create -o terrain-images.json ${DUNE2_TERRAIN_FILES}
)

# Miscellaneous
add_custom_command(
  OUTPUT "misc-images.json"
  DEPENDS
    ${DUNE2_MISC_FILES}
    RCToolkit
  COMMENT
    "Importing Misc tiles"
  COMMAND
    $<TARGET_FILE:RCToolkit>
      images create -o misc-images.json ${DUNE2_MISC_FILES}
)

add_custom_target(${PROJECT_NAME} ALL
  DEPENDS
    "palette.json"
    "units-images.json"
    "terrain-images.json"
    "misc-images.json"
)

foreach(DATA_JSON palette units-images terrain-images misc-images)
  add_custom_command(
    TARGET ${PROJECT_NAME}
    POST_BUILD
    COMMENT
      "Deflate ${DATA_JSON}.json"
    COMMAND
      gzip --force "${CMAKE_CURRENT_BINARY_DIR}/${DATA_JSON}.json"
  )
  install(
    FILES "${CMAKE_CURRENT_BINARY_DIR}/${DATA_JSON}.json.gz"
    DESTINATION public/assets
  )
endforeach()
