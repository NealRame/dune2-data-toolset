set(DUNE2_ATRE_PAK_FILES jeu-00022-dune_2-pcdos/Dune2/ATRE.PAK)
set(DUNE2_HARK_PAK_FILES jeu-00022-dune_2-pcdos/Dune2/HARK.PAK)
set(DUNE2_ORDOS_PAK_FILES jeu-00022-dune_2-pcdos/Dune2/ORDOS.PAK)
set(DUNE2_VOC_PAK_FILES jeu-00022-dune_2-pcdos/Dune2/VOC.PAK)
set(DUNE2_DUNE_PAK_FILES jeu-00022-dune_2-pcdos/Dune2/DUNE.PAK)

list(APPEND DUNE2_PAK_FILES
  ${DUNE2_ATRE_PAK_FILES}
  ${DUNE2_HARK_PAK_FILES}
  ${DUNE2_ORDOS_PAK_FILES}
  ${DUNE2_VOC_PAK_FILES}
  ${DUNE2_DUNE_PAK_FILES}
)

list(APPEND DUNE2_HARKONNEN_VOC_FILES
  HARK/HWEST.VOC
  HARK/HHARK.VOC
  HARK/HMERC.VOC
  HARK/HLOCATED.VOC
  HARK/HHARVEST.VOC
  HARK/HWORMY.VOC
  HARK/HMISSILE.VOC
  HARK/HATTACK.VOC
  HARK/HDESTROY.VOC
  HARK/HWARNING.VOC
  HARK/HSARD.VOC
  HARK/HBLOOM.VOC
  HARK/HONE.VOC
  HARK/HSTRUCT.VOC
  HARK/HSABOT.VOC
  HARK/HFOUR.VOC
  HARK/HUNIT.VOC
  HARK/HAPPRCH.VOC
  HARK/HLOSE.VOC
  HARK/HREPAIR.VOC
  HARK/HEAST.VOC
  HARK/HLAUNCH.VOC
  HARK/HORDOS.VOC
  HARK/HFIVE.VOC
  HARK/HFREMEN.VOC
  HARK/HTWO.VOC
  HARK/HSOUTH.VOC
  HARK/HFIELD.VOC
  HARK/HFRIGATE.VOC
  HARK/HNEXT2.VOC
  HARK/HRADAR.VOC
  HARK/HWIN.VOC
  HARK/HNORTH.VOC
  HARK/HTHREE.VOC
  HARK/HVEHICLE.VOC
  HARK/HOFF.VOC
  HARK/HDEPLOY.VOC
  HARK/HCONST.VOC
  HARK/HON.VOC
  HARK/HNEXT.VOC
  HARK/HENEMY.VOC
  HARK/HARRIVE.VOC
  HARK/HCAPTURE.VOC
  HARK/HATRE.VOC
)

list(APPEND DUNE2_ORDOS_VOC_FILES
  ORDOS/OLAUNCH.VOC
  ORDOS/OREPAIR.VOC
  ORDOS/OOFF.VOC
  ORDOS/OFIVE.VOC
  ORDOS/OBLOOM.VOC
  ORDOS/OSTRUCT.VOC
  ORDOS/OSABOT.VOC
  ORDOS/OAPPRCH.VOC
  ORDOS/OTWO.VOC
  ORDOS/OWARNING.VOC
  ORDOS/OATTACK.VOC
  ORDOS/ODESTROY.VOC
  ORDOS/OATRE.VOC
  ORDOS/OMISSILE.VOC
  ORDOS/OHARVEST.VOC
  ORDOS/OWORMY.VOC
  ORDOS/OLOCATED.VOC
  ORDOS/ONEXT.VOC
  ORDOS/OWIN.VOC
  ORDOS/OONE.VOC
  ORDOS/OENEMY.VOC
  ORDOS/OCAPTURE.VOC
  ORDOS/OON.VOC
  ORDOS/OHARK.VOC
  ORDOS/OARRIVE.VOC
  ORDOS/OMERC.VOC
  ORDOS/OWEST.VOC
  ORDOS/ONORTH.VOC
  ORDOS/OVEHICLE.VOC
  ORDOS/ODEPLOY.VOC
  ORDOS/OTHREE.VOC
  ORDOS/OCONST.VOC
  ORDOS/OLOSE.VOC
  ORDOS/OFOUR.VOC
  ORDOS/OUNIT.VOC
  ORDOS/ONEXT2.VOC
  ORDOS/OSARD.VOC
  ORDOS/ORADAR.VOC
  ORDOS/OORDOS.VOC
  ORDOS/OFREMEN.VOC
  ORDOS/OFRIGATE.VOC
  ORDOS/OFIELD.VOC
  ORDOS/OSOUTH.VOC
  ORDOS/OEAST.VOC
)

list(APPEND DUNE2_ATREIDES_VOC_FILES
  ATRE/AHARK.VOC
  ATRE/ANEXT2.VOC
  ATRE/AMERC.VOC
  ATRE/AWEST.VOC
  ATRE/AFRIGATE.VOC
  ATRE/ARADAR.VOC
  ATRE/AORDOS.VOC
  ATRE/AOFF.VOC
  ATRE/AFIELD.VOC
  ATRE/ASOUTH.VOC
  ATRE/AFREMEN.VOC
  ATRE/AFOUR.VOC
  ATRE/AUNIT.VOC
  ATRE/AARRIVE.VOC
  ATRE/ALOSE.VOC
  ATRE/AENEMY.VOC
  ATRE/AWIN.VOC
  ATRE/ASARD.VOC
  ATRE/AVEHICLE.VOC
  ATRE/ANORTH.VOC
  ATRE/ADEPLOY.VOC
  ATRE/ACAPTURE.VOC
  ATRE/ATHREE.VOC
  ATRE/ATWO.VOC
  ATRE/AON.VOC
  ATRE/ACONST.VOC
  ATRE/AEAST.VOC
  ATRE/AHARVEST.VOC
  ATRE/ALOCATED.VOC
  ATRE/AATTACK.VOC
  ATRE/AFIVE.VOC
  ATRE/AMISSILE.VOC
  ATRE/AWARNING.VOC
  ATRE/AWORMY.VOC
  ATRE/ADESTROY.VOC
  ATRE/AONE.VOC
  ATRE/AREPAIR.VOC
  ATRE/ALAUNCH.VOC
  ATRE/ABLOOM.VOC
  ATRE/AAPPRCH.VOC
  ATRE/AATRE.VOC
  ATRE/ASABOT.VOC
  ATRE/ASTRUCT.VOC
  ATRE/ANEXT.VOC
)

list(APPEND DUNE2_GAME_FX_VOC_FILES
  FX/ZMOVEOUT.VOC
  FX/DROPEQ2P.VOC
  FX/EXGAS.VOC
  FX/EXCANNON.VOC
  FX/BUTTON.VOC
  FX/MISLTINP.VOC
  FX/WORMET3P.VOC
  FX/ROCKET.VOC
  FX/EXSAND.VOC
  FX/EXDUD.VOC
  FX/EXMED.VOC
  FX/SANDBUG.VOC
  FX/EXTINY.VOC
  FX/EXLARGE.VOC
  FX/EXSMALL.VOC
  FX/ZREPORT1.VOC
  FX/GUNMULTI.VOC
  FX/ZREPORT3.VOC
  FX/GUN.VOC
  FX/CRUMBLE.VOC
  FX/ZREPORT2.VOC
  FX/ZOVEROUT.VOC
  FX/VSCREAM3.VOC
  FX/VSCREAM2.VOC
  FX/SQUISH2.VOC
  FX/POPPA.VOC
  FX/VSCREAM1.VOC
  FX/VSCREAM5.VOC
  FX/VSCREAM4.VOC
  FX/STATICP.VOC
  FX/ZAFFIRM.VOC
)

set(DUNE2_PALETTE_OUTPUT_FILE "palette.json")
set(DUNE2_PALETTE_SOURCE GFX/BENE.PAL)

set(DUNE2_IMAGES_MISC_OUTPUT_FILE "images.misc.json")
list(APPEND DUNE2_IMAGES_MISC_SOURCES
  GFX/SHAPES.SHP
)

set(DUNE2_IMAGES_TERRAIN_OUTPUT_FILE "images.terrain.json")
list(APPEND DUNE2_IMAGES_TERRAIN_SOURCES
  GFX/ICON.ICN
)

set(DUNE2_IMAGES_UNITS_OUTPUT_FILE "images.units.json")
list(APPEND DUNE2_IMAGES_UNITS_SOURCES
  GFX/UNITS.SHP
  GFX/UNITS1.SHP
  GFX/UNITS2.SHP
)

set(DUNE2_TILES_OUTPUT_FILE "tiles.json")
set(DUNE2_TILES_TERRAIN_SOURCES GFX/ICON.MAP)

list(APPEND DUNE2_DATA_SOURCES
  ${DUNE2_PALETTE_SOURCE}
  ${DUNE2_IMAGES_MISC_SOURCES}
  ${DUNE2_IMAGES_TERRAIN_SOURCES}
  ${DUNE2_IMAGES_UNITS_SOURCES}
  ${DUNE2_TILES_TERRAIN_SOURCES}
)

list(APPEND DUNE2_DATA_OUTPUT_FILES
  ${DUNE2_PALETTE_OUTPUT_FILE}
  ${DUNE2_IMAGES_MISC_OUTPUT_FILE}
  ${DUNE2_IMAGES_TERRAIN_OUTPUT_FILE}
  ${DUNE2_IMAGES_UNITS_OUTPUT_FILE}
  ${DUNE2_TILES_OUTPUT_FILE}
)

