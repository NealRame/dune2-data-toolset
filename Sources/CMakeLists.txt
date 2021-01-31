find_program(PROTOC_BIN protoc REQUIRED)

set(DUNE2_RC_PROTO "dune2_rc.proto")
set(DUNE2_RC_PROTO_DIR ${CMAKE_CURRENT_SOURCE_DIR})

set(DUNE2_RC "dune2.rc")
set(DUNE2_RC_PB_JS "dune2_rc_pb.js")

add_subdirectory(PAKExtract)
add_subdirectory(Dune2)
add_subdirectory(RCToolkit)

project(Dune2RC)

include(Dune2DataFiles.cmake)

if(NOT EXISTS ${DUNE2_DATA_ARCHIVE})
  message(FATAL_ERROR "'${DUNE2_DATA_ARCHIVE}' does not exists!")
endif()

add_custom_target(${PROJECT_NAME}
  ALL
  BYPRODUCTS
    ${DUNE2_RC}
    ${DUNE2_RC_PB_JS}
  DEPENDS
    ${DUNE2_ATREIDES_MP3_FILES}
    ${DUNE2_HARKONNEN_MP3_FILES}
    ${DUNE2_ORDOS_MP3_FILES}
    ${DUNE2_GAME_FX_MP3_FILES}
    ${DUNE2_TILES_FILES}
  COMMAND
    ${CMAKE_COMMAND} -E remove -f ${DUNE2_RC} ${DUNE2_RC_PB_JS}
)

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
  OUTPUT ${DUNE2_TILES_FILES}
  DEPENDS
    PAKExtract
    ${DUNE2_DUNE_PAK_FILES}
  COMMENT
    "Extracting graphism files"
  COMMAND
    $<TARGET_FILE:PAKExtract> -d GFX ${DUNE2_DUNE_PAK_FILES}
)

# Convert VOC to MP3
foreach(VOC_FILE
    ${DUNE2_HARKONNEN_VOC_FILES}
    ${DUNE2_ORDOS_VOC_FILES}
    ${DUNE2_ATREIDES_VOC_FILES}
    ${DUNE2_GAME_FX_VOC_FILES})
  string(REGEX REPLACE
    "VOC$" "mp3"
    MP3_FILE
    ${VOC_FILE}
  )
  add_custom_command(
    OUTPUT
      ${MP3_FILE}
    DEPENDS
      ${VOC_FILE}
    COMMENT
      "Converting ${VOC_FILE} to ${MP3_FILE}"
    COMMAND
      ffmpeg -nostdin -y -hide_banner -loglevel error -i "${VOC_FILE}" "${MP3_FILE}" || true
  )
endforeach()

# Atreides soundset
add_custom_command(
  TARGET ${PROJECT_NAME}
  POST_BUILD
  DEPENDS
    RCToolkit
    ${DUNE2_ATREIDES_MP3_FILES}
  COMMENT
    "Importing Atreides soundset"
  COMMAND
    $<TARGET_FILE:RCToolkit>
      --rc-file ${DUNE2_RC}
      soundset import
        --name Atreides ATRE
)

# Harkonnen soundset
add_custom_command(
  TARGET ${PROJECT_NAME}
  POST_BUILD
  DEPENDS
    RCToolkit
    ${DUNE2_HARKONNEN_MP3_FILES}
  COMMENT
    "Importing Harkonnen soundset"
  COMMAND
    $<TARGET_FILE:RCToolkit>
      --rc-file ${DUNE2_RC}
      soundset import
        --name Harkonnen HARK
)

# Harkonnen soundset
add_custom_command(
  TARGET ${PROJECT_NAME}
  POST_BUILD
  DEPENDS
    RCToolkit
    ${DUNE2_ORDOS_MP3_FILES}
  COMMENT
    "Importing Harkonnen soundset"
  COMMAND
    $<TARGET_FILE:RCToolkit>
      --rc-file ${DUNE2_RC}
      soundset import
        --name Ordos ORDOS
)

# Audio FX soundset
add_custom_command(
  TARGET ${PROJECT_NAME}
  POST_BUILD
  DEPENDS
    RCToolkit
    ${DUNE2_GAME_FX_MP3_FILES}
  COMMENT
    "Importing AudioFX soundset"
  COMMAND
    $<TARGET_FILE:RCToolkit>
      --rc-file ${DUNE2_RC}
      soundset import
        --name AudioFX FX
)

# palette
add_custom_command(
  TARGET ${PROJECT_NAME}
  POST_BUILD
  DEPENDS
    RCToolkit
    ${DUNE2_PAL_FILE}
  COMMENT
    "Importing Bene palette"
  COMMAND
    $<TARGET_FILE:RCToolkit>
      --rc-file ${DUNE2_RC}
      palette import
        --name Bene ${DUNE2_PAL_FILE}
)

# Units tileset
add_custom_command(
  TARGET ${PROJECT_NAME}
  POST_BUILD
  DEPENDS
    RCToolkit
    ${DUNE2_UNITS_FILES}
  COMMENT
    "Importing Units tiles"
  COMMAND
    $<TARGET_FILE:RCToolkit>
      --rc-file ${DUNE2_RC}
      tileset import
        --name Units ${DUNE2_UNITS_FILES}
)

# Terrain tileset
add_custom_command(
  TARGET ${PROJECT_NAME}
  POST_BUILD
  DEPENDS
    RCToolkit
    ${DUNE2_TERRAIN_FILES}
  COMMENT
    "Importing Terrain tiles"
  COMMAND
    $<TARGET_FILE:RCToolkit>
      --rc-file ${DUNE2_RC}
      tileset import
        --name Terrain ${DUNE2_TERRAIN_FILES}
)

# Javascript RC lib
add_custom_command(
  TARGET ${PROJECT_NAME}
  POST_BUILD
  DEPENDS
    ${DUNE2_RC_PROTO}
  COMMENT "Generating javascript RC library"
  COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_CURRENT_BINARY_DIR}"
  COMMAND
    "${PROTOC_BIN}"
        --proto_path=${DUNE2_RC_PROTO_DIR}
        --js_out=import_style=commonjs,binary:"${CMAKE_CURRENT_BINARY_DIR}"
        ${DUNE2_RC_PROTO}
  USES_TERMINAL
)

install(
  FILES ${CMAKE_CURRENT_BINARY_DIR}/${DUNE2_RC}
  DESTINATION public/assets
)
install(
  FILES ${CMAKE_CURRENT_BINARY_DIR}/${DUNE2_RC_PB_JS}
  DESTINATION src/core
  RENAME dune2-rc.js
)
