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
    ${DUNE2_DATA_SOURCES}
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
  OUTPUT ${DUNE2_PALETTE_OUTPUT_FILE}
  DEPENDS
    ${DUNE2_PALETTE_SOURCE}
    RCToolkit
  COMMENT
    "Create palette"
  COMMAND
    $<TARGET_FILE:RCToolkit>
      palette create -o ${DUNE2_PALETTE_OUTPUT_FILE} ${DUNE2_PALETTE_SOURCE}
)

# Miscellaneous image set
add_custom_command(
  OUTPUT ${DUNE2_IMAGES_MISC_OUTPUT_FILE}
  DEPENDS
    ${DUNE2_IMAGES_MISC_SOURCES}
    RCToolkit
  COMMENT
    "Importing Misc tiles"
  COMMAND
    $<TARGET_FILE:RCToolkit>
      images create -o ${DUNE2_IMAGES_MISC_OUTPUT_FILE} ${DUNE2_IMAGES_MISC_SOURCES}
)

# Terrain image set
add_custom_command(
  OUTPUT ${DUNE2_IMAGES_TERRAIN_OUTPUT_FILE}
  DEPENDS
    ${DUNE2_IMAGES_TERRAIN_SOURCES}
    RCToolkit
  COMMENT
    "Importing Terrain images"
  COMMAND
    $<TARGET_FILE:RCToolkit>
      images create -o ${DUNE2_IMAGES_TERRAIN_OUTPUT_FILE} ${DUNE2_IMAGES_TERRAIN_SOURCES}
)

# Units image set
add_custom_command(
  OUTPUT ${DUNE2_IMAGES_UNITS_OUTPUT_FILE}
  DEPENDS
    ${DUNE2_IMAGES_UNITS_SOURCES}
    RCToolkit
  COMMENT
    "Importing Units images"
  COMMAND
    $<TARGET_FILE:RCToolkit>
      images create -o ${DUNE2_IMAGES_UNITS_OUTPUT_FILE} ${DUNE2_IMAGES_UNITS_SOURCES}
)

add_custom_target(${PROJECT_NAME}
  ALL
  DEPENDS
    ${DUNE2_DATA_OUTPUT_FILES}
)

foreach(DATA_JSON ${DUNE2_DATA_OUTPUT_FILES})
  add_custom_command(
    TARGET ${PROJECT_NAME}
    POST_BUILD
    COMMENT
      "Deflate ${DATA_JSON}"
    COMMAND
      gzip --force "${DATA_JSON}"
  )
  install(
    FILES "${CMAKE_CURRENT_BINARY_DIR}/${DATA_JSON}.gz"
    DESTINATION public/assets
  )
endforeach()
