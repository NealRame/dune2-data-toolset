#! /usr/bin/env bash

set -e
set -u

VERBOSE=false

function usage() {
    1>&2 cat <<-EOF
Usage: $(basename "$0") [OPTIONS] <DUNE2_DATA_BASE_DIR_PATH>

Options:
  -h        Print this help message and exit
  -v        Enable verbose output
EOF
}

while getopts "chv" o;
do
    case "${o}" in
    h)
        usage
        exit
        ;;
    v)
        VERBOSE=true
        ;;
    *)
        cat <<-EOF
Run with -h for more information.
EOF
        exit 1
        ;;
    esac
done

shift $((OPTIND-1))

if [ "$#" -ne 1 ] || ! [ -d "$1" ]; then
    1>&2 cat <<-EOF
** missing directory!
Run with --help for more information.
EOF
    exit 1
fi

DUNE2_DATA_BASE_DIR_PATH="$1"

$VERBOSE && echo "Remove dune2.rc"
rm -fr dune2.rc

while read -r RC_TYPE RC_NAME RC_SOURCE;
do
    RC_PATH="$DUNE2_DATA_BASE_DIR_PATH/$RC_SOURCE"

    $VERBOSE && echo "Importing '$RC_SOURCE' to $RC_TYPE '$RC_NAME'"

    if ! [ -e "$RC_PATH" ];
    then
        1>&2 echo "** missing '$RC_SOURCE'!"
        exit 1
    fi

    dune2-rc-toolkit \
        --rc-file dune2.rc \
        "$RC_TYPE" import --name "$RC_NAME" "$RC_PATH"
done < <(cat <<EOF
soundset Atreides ATRE.PAK
soundset Harkonnen HARK.PAK
soundset Ordos ORDOS.PAK
palette Bene DUNE.PAK/BENE.PAL
tileset Terrain DUNE.PAK/ICON.ICN
tileset Units DUNE.PAK/UNITS.SHP
tileset Units DUNE.PAK/UNITS1.SHP
tileset Units DUNE.PAK/UNITS2.SHP
EOF
)
