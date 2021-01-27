#! /usr/bin/env bash

set -e
set -u

VERBOSE=false
REMOVE_VOC_FILE=false

function voc_convert_to_mp3() {
    local VOC_SCAN_DIR="$1"

    find "$VOC_SCAN_DIR" -iname "*.VOC" | while read -r VOC;
    do
        MP3_DIR="$(dirname "$VOC")"
        MP3="$MP3_DIR/$(basename "$VOC" .VOC).mp3"

        mkdir -p "$MP3_DIR"

        $VERBOSE && echo "converting $VOC ..."
        ffmpeg -nostdin -y -hide_banner -loglevel error -i "$VOC" "$MP3" || true
        $REMOVE_VOC_FILE && rm -fr "$VOC"
    done
}

function usage() {
    1>&2 cat <<-EOF
Usage: $(basename "$0") [OPTIONS] DIRPATH

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
    c)
        REMOVE_VOC_FILE=true
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
$0: missing directory
Run with --help for more information.
EOF
    exit 1
fi

voc_convert_to_mp3 "$1"
