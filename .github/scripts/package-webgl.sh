#!/usr/bin/env bash
set -euo pipefail
[[ $# == 3 ]] || { echo 'Usage: package-webgl.sh <project> <ludork-artifact> <output>' >&2; exit 1; }
source "$(dirname "$0")/package-macos-common.sh"
init_package webgl "$@"

packer="$work/BuildTools/pack_web.sh"
if [[ ! -f "$packer" ]]; then
    echo "Ludork BuildTools $LUDORK_SHA does not include pack_web.sh." >&2
    exit 1
fi
args=(--emsdk "$EMSDK" --dev --encrypt-data --compile-lua --use-ldpak "$project" "$output")
sh "$packer" --check "${args[@]}"
sh "$packer" "${args[@]}"

packages=("$output"/*-webgl)
[[ ${#packages[@]} == 1 && -d "${packages[0]}" ]]
site=${packages[0]}
for name in index.html Main.js Main.data Main.wasm LICENSE.md ThirdPartySource/FFmpeg-Build/build_web.sh; do
    test -s "$site/$name"
done
[[ "$(od -An -tx1 -N4 "$site/Main.wasm" | tr -d ' \n')" == 0061736d ]]
test -d "$site/Licenses"
sources=("$site"/ThirdPartySource/ffmpeg-*.tar.gz)
[[ ${#sources[@]} == 1 && -s "${sources[0]}" ]]
python3 "$ci" validate "$project/build-web/resources"
python3 "$ci" metadata "$project" "$site" webgl
printf 'site=%s\n' "$site" >> "$GITHUB_OUTPUT"
